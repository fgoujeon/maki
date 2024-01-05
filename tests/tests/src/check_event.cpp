//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct context{};

    namespace states
    {
        EMPTY_STATE(on)
        EMPTY_STATE(off)
    }

    namespace events
    {
        struct button_press
        {
            bool hard = false;
        };
    }

    namespace guards
    {
        bool is_pressing_hard(const context& /*ctx*/, const events::button_press& event)
        {
            return event.hard;
        }
    }

    constexpr auto transition_table = maki::transition_table{}
        .add_c<states::off, events::button_press, states::on,  maki::noop, guards::is_pressing_hard>
        .add_c<states::on,  events::button_press, states::off, maki::noop>
    ;

    struct machine_conf_holder
    {
        static constexpr auto conf = maki::machine_conf{}
            .transition_tables(transition_table)
            .context<context>()
        ;
    };

    using machine_t = maki::machine<machine_conf_holder>;
}

TEST_CASE("check_event")
{
    auto machine = machine_t{};
    machine.start();

    REQUIRE(machine.active_state<states::off>());
    REQUIRE(!machine.check_event(events::button_press{})); // should fail guard
    REQUIRE(machine.check_event(events::button_press{true}));
    REQUIRE(machine.active_state<states::off>());

    machine.process_event(events::button_press{true});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(machine.check_event(events::button_press{}));
    REQUIRE(machine.active_state<states::on>());
}

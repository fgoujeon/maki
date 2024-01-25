//Copyright Florian Goujeon 2021 - 2024.
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
        (states::off, maki::type<events::button_press>, states::on,  maki::null, guards::is_pressing_hard)
        (states::on,  maki::type<events::button_press>, states::off, maki::null)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
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

//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace action_ns
{
    struct context
    {
        void boop()
        {
            i = 0;
        }

        int i = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(on)
    }

    namespace actions
    {
        constexpr auto beep = maki::action_c([](context& ctx)
        {
            ctx.i = 1;
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, states::on,  maki::event<events::button_press>, actions::beep)
        (states::on,  states::off, maki::event<events::button_press>, maki::action_c(&context::boop))
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("action")
{
    using namespace action_ns;

    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.context().i == 1);

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::off>());
    REQUIRE(machine.context().i == 0);
}

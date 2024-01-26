//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
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
        void beep(context& ctx)
        {
            ctx.i = 1;
        }
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on,  actions::beep)
        (states::on,  maki::type<events::button_press>, states::off, &context::boop)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("action")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(machine.context().i == 1);

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::off>());
    REQUIRE(machine.context().i == 0);
}

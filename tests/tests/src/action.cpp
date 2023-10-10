//Copyright Florian Goujeon 2021 - 2023.
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
        int i = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(on);
    }

    namespace actions
    {
        void beep(context& ctx)
        {
            ctx.i = 1;
        }

        void boop(context& ctx)
        {
            ctx.i = 0;
        }
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on,  actions::beep>
        .add_c<states::on,  events::button_press, states::off, actions::boop>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context_type<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("action")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(machine.context().i == 1);

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE(machine.context().i == 0);
}

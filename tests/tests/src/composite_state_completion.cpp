//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace composite_state_completion_ns
{
    struct context
    {
        int i = 0;
    };

    namespace events
    {
        struct button_press{};
        struct other_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(waiting)

        constexpr auto running_transition_table_0 = maki::transition_table{}
            (maki::ini,  waiting)
            (waiting,    maki::fin,   maki::event<events::button_press>)
        ;

        constexpr auto running_transition_table_1 = maki::transition_table{}
            (maki::ini,  waiting)
            (waiting,    maki::fin,   maki::event<events::other_button_press>)
        ;

        constexpr auto running = maki::state_mold{}
            .transition_tables(running_transition_table_0, running_transition_table_1)
        ;
    }

    namespace actions
    {
        constexpr auto beep = maki::action_c([](context& ctx)
        {
            ++ctx.i;
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,       states::running)
        (states::running, maki::fin,   maki::null, actions::beep)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("composite_state_completion_ns")
{
    using namespace composite_state_completion_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    CHECK(machine.is<states::running>());
    CHECK(machine.state<states::running>().region<0>().is<states::waiting>());
    CHECK(machine.state<states::running>().region<1>().is<states::waiting>());
    CHECK(ctx.i == 0);

    machine.process_event(events::other_button_press{});
    CHECK(machine.is<states::running>());
    CHECK(machine.state<states::running>().region<0>().is<states::waiting>());
    CHECK(!machine.state<states::running>().region<1>().is<states::waiting>());
    CHECK(ctx.i == 0);

    machine.process_event(events::button_press{});
    CHECK(!machine.is<states::running>());
    CHECK(ctx.i == 1);
}

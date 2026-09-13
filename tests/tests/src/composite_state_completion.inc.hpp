//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(composite_state_completion_ns)
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

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::AOS(machine)<machine_conf>;

    AOS_TASK_TYPE(void) test()
    {
        auto machine = machine_t{};
        auto& ctx = machine.context();

#if AOS_ASYNC
        co_await machine.start();
#endif

        CHECK(machine.is<states::running>());
        CHECK(machine.state<states::running>().region<0>().is<states::waiting>());
        CHECK(machine.state<states::running>().region<1>().is<states::waiting>());
        CHECK(ctx.i == 0);

        AOS_CALL machine.process_event(events::other_button_press{});
        CHECK(machine.is<states::running>());
        CHECK(machine.state<states::running>().region<0>().is<states::waiting>());
        CHECK(!machine.state<states::running>().region<1>().is<states::waiting>());
        CHECK(ctx.i == 0);

        AOS_CALL machine.process_event(events::button_press{});
        CHECK(!machine.is<states::running>());
        CHECK(ctx.i == 1);
    }
}

AOS_TEST_CASE(composite_state_completion)

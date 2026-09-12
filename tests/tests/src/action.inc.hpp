//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(action_ns)
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
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>, actions::beep)
        (states::on,  states::off, maki::event<events::button_press>, maki::action_c(&context::boop))
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

        AOS_CALL machine.start();

        AOS_CALL machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::on>());
        REQUIRE(machine.context().i == 1);

        AOS_CALL machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::off>());
        REQUIRE(machine.context().i == 0);
    }
}

AOS_TEST_CASE(action)

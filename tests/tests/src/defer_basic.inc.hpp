//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(defer_basic_ns)
{
    struct context
    {
        bool button_pressed = false;
    };

    namespace events
    {
        struct end_of_init{};
        struct button_press{};
    }

    namespace states
    {
        constexpr auto initializing = maki::state_mold{}
            .defer<events::button_press>();

        constexpr auto running = maki::state_mold{}
            .internal_action_c<events::button_press>(
                [](context& ctx)
                {
                    ctx.button_pressed = true;
                });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,            states::initializing)
        (states::initializing, states::running, maki::event<events::end_of_init>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST(defer_basic)
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(!machine.context().button_pressed);

        AOS_CALL machine.AOS(process_event)(events::end_of_init{});
        REQUIRE(machine.context().button_pressed);
    }
}

//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSEaÉBPUPÉB b"()_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(defer_chain_ns)
{
    struct context
    {
        bool button_pressed = false;
    };

    namespace events
    {
        struct e1{};
        struct e2{};
        struct e3{};
    }

    namespace states
    {
        constexpr auto s1 = maki::state_mold{}
            .defer<events::e1>()
            .defer<events::e2>();

        constexpr auto s2 = maki::state_mold{}
            .defer<events::e1>();

        constexpr auto s3 = maki::state_mold{};

        constexpr auto s4 = maki::state_mold{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,  states::s1)
        (states::s1, states::s2, maki::event<events::e3>)
        (states::s2, states::s3, maki::event<events::e2>)
        (states::s3, states::s4, maki::event<events::e1>)
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

    AOS_TEST(defer_chain)
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();

        AOS_CALL machine.AOS(process_event)(events::e1{});
        AOS_CALL machine.AOS(process_event)(events::e2{});
        AOS_CALL machine.AOS(process_event)(events::e3{});
        REQUIRE(machine.is<states::s4>());
    }
}

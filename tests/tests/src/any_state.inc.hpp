//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(any_state_ns)
{
    struct context{};

    namespace states
    {
        EMPTY_STATE(idle)
        EMPTY_STATE(running)
        EMPTY_STATE(failed)
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,        states::idle)
        (states::idle,     states::running, maki::event<events::start_button_press>)
        (states::running,  states::idle,    maki::event<events::stop_button_press>)
        (states::failed,   states::idle,    maki::event<events::stop_button_press>)
        (maki::all_states, states::failed,  maki::event<events::error>)
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

    AOS_TASK_TYPE(void) test()
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();

        AOS_CALL machine.AOS(process_event)(events::stop_button_press{});
        AOS_CALL machine.AOS(process_event)(events::error{});
        REQUIRE(machine.is<states::failed>());

        AOS_CALL machine.AOS(process_event)(events::stop_button_press{});
        AOS_CALL machine.AOS(process_event)(events::start_button_press{});
        AOS_CALL machine.AOS(process_event)(events::error{});
        REQUIRE(machine.is<states::failed>());
    }
}

AOS_TEST_CASE(any_state)

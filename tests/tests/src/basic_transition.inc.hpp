//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace AOS(basic_transition_ns)
{
    struct context
    {
    };

    namespace states
    {
        EMPTY_STATE(on)
        EMPTY_STATE(off)
    }

    namespace events
    {
        struct button_press{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .auto_start(false)
            .run_to_completion(false)
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::AOS(machine)<machine_conf>;

    AOS_TASK_TYPE(void) test()
    {
        auto machine = machine_t{};

        REQUIRE(!machine.running());

        AOS_CALL machine.start();
        REQUIRE(machine.is<states::off>());

        AOS_CALL machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::on>());

        AOS_CALL machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::off>());
    }
}

AOS_TEST_CASE(basic_transition)

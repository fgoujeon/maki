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
    };

    namespace events
    {
        struct button_press{};
        struct off_button_press{};
        struct destruction_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(s0_sub)
        EMPTY_STATE(s1)

        constexpr auto s0_transition_table = maki::empty_transition_table
            .add_c<s0_sub, events::button_press, maki::null>
        ;

        constexpr auto s0 = maki::submachine_conf
            .transition_tables(s0_transition_table)
        ;
    }

    constexpr auto any_but_s0_s1 = maki::any_but<states::s0, states::s1>;
    constexpr auto any_of_s0_s1 = maki::any_of<states::s0, states::s1>;

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off,   events::button_press,             states::s0>
        .add_c<states::s0,    events::button_press,             states::s1>
        .add_c<any_but_s0_s1, events::off_button_press,         states::off>
        .add_c<any_of_s0_s1,  events::destruction_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .transition_tables(transition_table)
            .context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("submachine_in_type_pattern")
{
    auto machine = machine_t{};

    machine.start();
    REQUIRE(machine.is_active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::s0>());

    machine.process_event(events::off_button_press{});
    REQUIRE(machine.is_active_state<states::s0>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::s1>());

    machine.process_event(events::off_button_press{});
    REQUIRE(machine.is_active_state<states::s1>());

    machine.process_event(events::destruction_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
}

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

        constexpr auto s0_transition_table = maki::transition_table{}
            (s0_sub, maki::type<events::button_press>, maki::null_c)
        ;

        constexpr auto s0 = maki::submachine_conf{}
            .transition_tables(s0_transition_table)
        ;
    }

    constexpr auto any_but_s0_s1 = maki::any_but<states::s0, states::s1>;
    constexpr auto any_of_s0_s1 = maki::any_of<states::s0, states::s1>;

    constexpr auto transition_table = maki::transition_table{}
        (states::off,   maki::type<events::button_press>,             states::s0)
        (states::s0,    maki::type<events::button_press>,             states::s1)
        (any_but_s0_s1, maki::type<events::off_button_press>,         states::off)
        (any_of_s0_s1,  maki::type<events::destruction_button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("submachine_in_type_pattern")
{
    auto machine = machine_t{};

    machine.start();
    REQUIRE(machine.active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::s0>());

    machine.process_event(events::off_button_press{});
    REQUIRE(machine.active_state<states::s0>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::s1>());

    machine.process_event(events::off_button_press{});
    REQUIRE(machine.active_state<states::s1>());

    machine.process_event(events::destruction_button_press{});
    REQUIRE(machine.active_state<states::off>());
}

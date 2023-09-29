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
        EMPTY_STATE(off);
        EMPTY_STATE(s0_sub);
        EMPTY_STATE(s1);

        using s0_transition_table = maki::transition_table
            ::add<s0_sub, events::button_press, maki::null>
        ;

        struct s0
        {
            static constexpr auto conf = maki::submachine_conf_c
                .set_transition_tables<s0_transition_table>()
            ;
        };
    }

    using any_but_s0_s1 = maki::any_but<states::s0, states::s1>;
    using any_of_s0_s1 = maki::any_of<states::s0, states::s1>;

    using transition_table_t = maki::transition_table
        ::add<states::off,   events::button_press,             states::s0>
        ::add<states::s0,    events::button_press,             states::s1>
        ::add<any_but_s0_s1, events::off_button_press,         states::off>
        ::add<any_of_s0_s1,  events::destruction_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
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

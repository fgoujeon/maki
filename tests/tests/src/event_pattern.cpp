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

    namespace states
    {
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    namespace events
    {
        struct power_button_press{};
        struct alert_button_press{};
    }

    [[maybe_unused]]
    constexpr auto make_sm_transition_table()
    {
        using any_button_press = maki::any_of
        <
            events::power_button_press,
            events::alert_button_press
        >;

        return maki::transition_table
            ::add<states::off, any_button_press,           states::on>
            ::add<states::on,  events::power_button_press, states::off>
        {};
    }

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<decltype(make_sm_transition_table())>
            ::context<context>
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("event_pattern")
{
    auto machine = machine_t{};

    REQUIRE(machine.is_active_state(states::off));

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state(states::on));

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is_active_state(states::off));

    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is_active_state(states::on));

    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is_active_state(states::on));
}

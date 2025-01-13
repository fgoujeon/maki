//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace event_pattern_ns
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
        struct power_button_press{};
        struct alert_button_press{};
    }

    [[maybe_unused]]
    constexpr auto make_sm_transition_table()
    {
        constexpr auto any_button_press = maki::any_event_of
        <
            events::power_button_press,
            events::alert_button_press
        >;

        return maki::transition_table{}
            (states::off, any_button_press,                        states::on)
            (states::on,  maki::event<events::power_button_press>, states::off)
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(make_sm_transition_table())
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("event_filter")
{
    using namespace event_pattern_ns;

    auto machine = machine_t{};

    REQUIRE(machine.is<states::off>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::on>());

    machine.process_event(events::power_button_press{});
    REQUIRE(machine.is<states::off>());

    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is<states::on>());

    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is<states::on>());
}

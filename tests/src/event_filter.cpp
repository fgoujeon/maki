//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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
        using any_button_press = awesm::any_of
        <
            events::power_button_press,
            events::alert_button_press
        >;

        return awesm::transition_table
            ::add<states::off, any_button_press,           states::on>
            ::add<states::on,  events::power_button_press, states::off>
        {};
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            awesm::sm_opts::transition_tables<decltype(make_sm_transition_table())>,
            awesm::sm_opts::context<context>
        >;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("event_filter")
{
    auto sm = sm_t{};

    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::alert_button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::alert_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
}

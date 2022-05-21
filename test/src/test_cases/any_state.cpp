//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context{};

    namespace states
    {
        struct idle{};
        struct running{};
        struct failed{};
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    struct fsm_conf: awesm::fsm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::idle,    events::start_button_press, states::running>,
            awesm::row<states::running, events::stop_button_press,  states::idle>,
            awesm::row<states::failed,  events::stop_button_press,  states::idle>,
            awesm::row<awesm::any,      events::error,              states::failed>
        >;
    };

    using fsm = awesm::fsm<fsm_conf>;
}

TEST_CASE("any state")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::start_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());
}

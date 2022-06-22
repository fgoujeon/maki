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

    struct sm_conf
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::idle,    events::start_button_press, states::running>,
            awesm::row<states::running, events::stop_button_press,  states::idle>,
            awesm::row<states::failed,  events::stop_button_press,  states::idle>,
            awesm::row<awesm::any,      events::error,              states::failed>
        >;
    };

    using sm_t = awesm::simple_sm<sm_conf>;
}

TEST_CASE("any state")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::start_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());
}

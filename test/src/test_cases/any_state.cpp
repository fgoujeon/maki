//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context{};

    namespace states
    {
        FGFSM_SIMPLE_STATE(idle)
        FGFSM_SIMPLE_STATE(running)
        FGFSM_SIMPLE_STATE(failed)
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using context = ::context;

        static auto make_transition_table()
        {
            return fgfsm::transition_table
            {
                fgfsm::make_row<states::idle,    events::start_button_press, states::running>(),
                fgfsm::make_row<states::running, events::stop_button_press,  states::idle>(),
                fgfsm::make_row<states::failed,  events::stop_button_press,  states::idle>(),
                fgfsm::make_row<fgfsm::any,      events::error,              states::failed>()
            };
        }
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
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

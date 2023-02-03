//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context{};

    namespace states
    {
        EMPTY_STATE(idle);
        EMPTY_STATE(running);
        EMPTY_STATE(failed);
    }

    namespace events
    {
        struct start_button_press{};
        struct stop_button_press{};
        struct error{};
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
        <
            awesm::row<states::idle,    events::start_button_press, states::running>,
            awesm::row<states::running, events::stop_button_press,  states::idle>,
            awesm::row<states::failed,  events::stop_button_press,  states::idle>,
            awesm::row<awesm::any,      events::error,              states::failed>
        >;
    }

    struct sm_def
    {
        using conf_type = awesm::sm_conf
        <
            sm_transition_table,
            context
        >;
    };

    using sm_t = awesm::sm<sm_def>;
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

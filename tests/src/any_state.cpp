//Copyright Florian Goujeon 2021 - 2023.
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

    using sm_transition_table = awesm::transition_table
        ::add<states::idle,    events::start_button_press, states::running>
        ::add<states::running, events::stop_button_press,  states::idle>
        ::add<states::failed,  events::stop_button_press,  states::idle>
        ::add<awesm::any,      events::error,              states::failed>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf_tpl
        <
            awesm::sm_opts::transition_tables<sm_transition_table>,
            awesm::sm_opts::context<context>
        >;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("any state")
{
    auto sm = sm_t{};

    sm.start();

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());

    sm.process_event(events::stop_button_press{});
    sm.process_event(events::start_button_press{});
    sm.process_event(events::error{});
    REQUIRE(sm.is_active_state<states::failed>());
}

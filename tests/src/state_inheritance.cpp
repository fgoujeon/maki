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

        struct off: on
        {
        };
    }

    namespace events
    {
        struct button_press{};
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::button_press, states::on>
            .add<states::on,  events::button_press, states::off>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
            ::transition_table<sm_transition_table>
            ::context<context>
        ;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("state_inheritance")
{
    auto sm = sm_t{};

    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

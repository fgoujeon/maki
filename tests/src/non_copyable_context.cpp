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
        std::unique_ptr<int> pi;
    };

    namespace states
    {
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    namespace events
    {
        struct button_press{};
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::button_press, states::on>
        ::add<states::on,  events::button_press, states::off>
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

TEST_CASE("non_copyable_context")
{
    auto sm = sm_t{};

    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

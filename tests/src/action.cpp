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
        int i = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(on);
    }

    namespace actions
    {
        void beep(context& ctx)
        {
            ctx.i = 1;
        }

        void boop(context& ctx)
        {
            ctx.i = 0;
        }
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::button_press, states::on,  actions::beep>
        ::add<states::on,  events::button_press, states::off, actions::boop>
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

TEST_CASE("action")
{
    auto sm = sm_t{};

    sm.start();

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.get_context().i == 1);

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(sm.get_context().i == 0);
}

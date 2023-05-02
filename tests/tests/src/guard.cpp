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
        bool has_power = false;
    };

    namespace states
    {
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    namespace events
    {
        struct button_press
        {
            bool hard = false;
        };
    }

    namespace guards
    {
        bool has_power(context& ctx)
        {
            return ctx.has_power;
        }

        bool is_pressing_hard(context& /*ctx*/, const events::button_press& event)
        {
            return event.hard;
        }

        bool always_false(context& /*ctx*/)
        {
            return false;
        }
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::button_press, states::on,  awesm::noop, guards::has_power>
        ::add<states::on,  events::button_press, states::off, awesm::noop, guards::always_false>
        ::add<states::on,  events::button_press, states::off, awesm::noop, guards::is_pressing_hard>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf_tpl
        <
            awesm::subsm_opts::transition_tables<sm_transition_table>,
            awesm::subsm_opts::context<context>
        >;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("guard")
{
    auto sm = sm_t{};

    sm.start();

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    sm.context().has_power = true;
    sm.process_event(events::button_press{true});
    REQUIRE(sm.is_active_state<states::on>());
}

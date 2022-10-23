//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"

namespace
{
    struct context
    {
        bool has_power = false;
    };

    namespace states
    {
        struct on{};
        struct off{};
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
        struct has_power
        {
            bool check()
            {
                return ctx.has_power;
            }

            context& ctx;
        };

        struct is_pressing_hard
        {
            bool check(const events::button_press& event)
            {
                return event.hard;
            }
        };

        struct always_false
        {
            bool check(const events::button_press&)
            {
                return false;
            }
        };
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::off, events::button_press, states::on,  awesm::none, guards::has_power>,
        awesm::row<states::on,  events::button_press, states::off, awesm::none, guards::always_false>,
        awesm::row<states::on,  events::button_press, states::off, awesm::none, guards::is_pressing_hard>
    >;

    struct sm_def
    {
        using conf = awesm::sm_conf<sm_transition_table>;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("guard")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    ctx.has_power = true;
    sm.process_event(events::button_press{true});
    REQUIRE(sm.is_active_state<states::on>());
}

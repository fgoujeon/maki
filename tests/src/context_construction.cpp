//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

    struct context
    {
        sm_t& sm;
        int i = 42;
    };

    struct on_context
    {
        sm_t& sm;
        context& parent;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(emitting_red);
        EMPTY_STATE(emitting_green);
        EMPTY_STATE(emitting_blue);

        using on_transition_table = awesm::transition_table
            ::add<states::emitting_red,   events::color_button_press, states::emitting_green>
            ::add<states::emitting_green, events::color_button_press, states::emitting_blue>
            ::add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            using conf = awesm::sm_conf
                ::transition_tables<on_transition_table>
                ::context<on_context>
            ;
        };
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::power_button_press, states::on>
        ::add<states::on,  events::power_button_press, states::off>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf
            ::transition_tables<sm_transition_table>
            ::context<context>
        ;
    };
}

TEST_CASE("context_construction")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    REQUIRE(&ctx.sm == &sm);
    REQUIRE(ctx.i == 42);
}

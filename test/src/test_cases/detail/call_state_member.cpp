//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/detail/call_state_member.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
        int i = 0;
    };

    struct event
    {
        int i = 0;
    };

    struct state_with_event
    {
        void on_entry(const fgfsm::event_ref& evt)
        {
            if(const event* pevt = evt.get<event>())
            {
                ctx.i = pevt->i;
            }
        }

        void on_event(const fgfsm::event_ref& evt)
        {
            if(const event* pevt = evt.get<event>())
            {
                ctx.i = pevt->i;
            }
        }

        void on_exit(const fgfsm::event_ref& evt)
        {
            if(const event* pevt = evt.get<event>())
            {
                ctx.i = pevt->i;
            }
        }

        context& ctx;
    };

    struct state_without_event
    {
        void on_entry()
        {
            ctx.i = 99;
        }

        void on_exit()
        {
            ctx.i = 99;
        }

        context& ctx;
    };

    struct state_without_anything
    {
        context& ctx;
    };
}

TEST_CASE("detail::call_state_member")
{
    auto ctx = context{};
    auto state0 = state_with_event{ctx};
    auto state1 = state_without_event{ctx};
    auto state2 = state_without_anything{ctx};

    ctx.i = -1;
    fgfsm::detail::call_on_entry(state0, event{0});
    REQUIRE(ctx.i == 0);

    ctx.i = -1;
    fgfsm::detail::call_on_entry(state1, event{1});
    REQUIRE(ctx.i == 99);

    ctx.i = -1;
    fgfsm::detail::call_on_entry(state2, event{2});
    REQUIRE(ctx.i == -1);

    ctx.i = -1;
    fgfsm::detail::call_on_event(state0, event{3});
    REQUIRE(ctx.i == 3);

    ctx.i = -1;
    fgfsm::detail::call_on_event(state1, event{4});
    REQUIRE(ctx.i == -1);

    ctx.i = -1;
    fgfsm::detail::call_on_event(state2, event{5});
    REQUIRE(ctx.i == -1);

    ctx.i = -1;
    fgfsm::detail::call_on_exit(state0, event{6});
    REQUIRE(ctx.i == 6);

    ctx.i = -1;
    fgfsm::detail::call_on_exit(state1, event{7});
    REQUIRE(ctx.i == 99);

    ctx.i = -1;
    fgfsm::detail::call_on_exit(state2, event{8});
    REQUIRE(ctx.i == -1);
}

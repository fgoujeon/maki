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
        std::string output;
    };

    struct event
    {
        int i = 0;
    };

    struct state_with_event
    {
        void on_entry(const event& e)
        {
            ctx.output = std::to_string(e.i);
        }

        void on_event(const event& e)
        {
            ctx.output = std::to_string(e.i);
        }

        void on_exit(const event& e)
        {
            ctx.output = std::to_string(e.i);
        }

        context& ctx;
    };

    struct state_without_event
    {
        void on_entry()
        {
            ctx.output = '-';
        }

        void on_event()
        {
            ctx.output = '-';
        }

        void on_exit()
        {
            ctx.output = '-';
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

    fgfsm::detail::call_on_entry(state0, event{0});
    REQUIRE(ctx.output == "0");

    fgfsm::detail::call_on_entry(state1, event{1});
    REQUIRE(ctx.output == "-");

    fgfsm::detail::call_on_entry(state2, event{2});
    REQUIRE(ctx.output == "-");

    fgfsm::detail::call_on_event(state0, event{3});
    REQUIRE(ctx.output == "3");

    fgfsm::detail::call_on_event(state1, event{4});
    REQUIRE(ctx.output == "-");

    fgfsm::detail::call_on_event(state2, event{5});
    REQUIRE(ctx.output == "-");

    fgfsm::detail::call_on_exit(state0, event{6});
    REQUIRE(ctx.output == "6");

    fgfsm::detail::call_on_exit(state1, event{7});
    REQUIRE(ctx.output == "-");

    fgfsm::detail::call_on_exit(state2, event{8});
    REQUIRE(ctx.output == "-");
}

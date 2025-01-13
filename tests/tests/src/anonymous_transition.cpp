//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace anonymous_transition_ns
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct go_on{};
    }

    namespace states
    {
        EMPTY_STATE(s0)
        EMPTY_STATE(s1)
        EMPTY_STATE(s2)
        EMPTY_STATE(s3)
        EMPTY_STATE(s4)
    }

    constexpr auto do_something = maki::action_ce([](context& ctx, const events::go_on& /*event*/)
    {
        ctx.out = "did something";
    });

    constexpr auto transition_table = maki::transition_table{}
        (states::s0, maki::event<events::go_on>, states::s1)
        (states::s1, maki::null,                 states::s2, do_something)
        (states::s2, maki::event<events::go_on>, states::s3)
        (states::s3, maki::null,                 states::s4)
        (states::s4, maki::null,                 states::s0)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("anonymous transition")
{
    using namespace anonymous_transition_ns;

    auto machine = machine_t{};

    machine.start();
    REQUIRE(machine.is<states::s0>());

    machine.process_event(events::go_on{});
    REQUIRE(machine.is<states::s2>());
    REQUIRE(machine.context().out == "did something");

    machine.context().out.clear();
    machine.process_event(events::go_on{});
    REQUIRE(machine.is<states::s0>());
}

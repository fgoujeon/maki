//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace transition_to_stopped_ns
{
    struct context{};

    struct some_event{};

    namespace states
    {
        EMPTY_STATE(running)
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::running, maki::event<some_event>, maki::stopped)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .auto_start(false)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("transition_to_stopped")
{
    using namespace transition_to_stopped_ns;

    auto machine = machine_t{};
    REQUIRE(!machine.running());

    machine.start();
    REQUIRE(machine.running());

    machine.process_event(some_event{});
    REQUIRE(!machine.running());
    REQUIRE(machine.is<maki::stopped>());
}

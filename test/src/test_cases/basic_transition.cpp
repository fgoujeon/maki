//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context{};

    namespace states
    {
        struct on
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };

        struct off
        {
            void on_entry(){}
            void on_exit(){}
            context& ctx;
        };
    }

    namespace events
    {
        struct button_press{};
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::button_press, states::on>,
        fgfsm::row<states::on,  events::button_press, states::off>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("basic transition")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());

#ifdef CATCH_CONFIG_ENABLE_BENCHMARKING
    BENCHMARK("process_event")
    {
        sm.process_event(events::button_press{});
    };
#endif
}

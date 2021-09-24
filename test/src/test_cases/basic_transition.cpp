//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
    struct context
    {
    };

    namespace states
    {
        struct on
        {
            template<class Event>
            void on_entry(const Event& event)
            {
            }

            template<class Event>
            void on_event(const Event& event)
            {
            }

            template<class Event>
            void on_exit(const Event& event)
            {
            }

            context& ctx;
        };

        struct off
        {
            template<class Event>
            void on_entry(const Event& event)
            {
            }

            template<class Event>
            void on_event(const Event& event)
            {
            }

            template<class Event>
            void on_exit(const Event& event)
            {
            }

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

    REQUIRE(sm.is_current_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_current_state<states::on>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_current_state<states::off>());
}

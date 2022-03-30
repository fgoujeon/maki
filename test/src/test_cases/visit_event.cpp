//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
        int i = 0;
        std::string str;
    };

    namespace events
    {
        struct int_data
        {
            int i = 0;
        };

        struct text_data
        {
            std::string str;
        };

        struct next{};
    }

    namespace states
    {
        FGFSM_SIMPLE_STATE(idle);

        struct replacing_values
        {
            void on_entry(const fgfsm::any_cref&){}

            void on_event(const fgfsm::any_cref& event)
            {
                visit
                (
                    event,
                    [this](const events::int_data& event)
                    {
                        ctx.i = event.i;
                    },
                    [this](const events::text_data& event)
                    {
                        ctx.str = event.str;
                    }
                );
            }

            void on_exit(const fgfsm::any_cref&){}

            context& ctx;
        };

        struct cumulating_values
        {
            void on_entry(const fgfsm::any_cref&){}

            void on_event(const fgfsm::any_cref& event)
            {
                visit
                (
                    event,
                    [this](const events::int_data& event)
                    {
                        ctx.i += event.i;
                    },
                    [this](const events::text_data& event)
                    {
                        ctx.str += event.str;
                    }
                );
            }

            void on_exit(const fgfsm::any_cref&){}

            context& ctx;
        };

    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle,              events::next, states::replacing_values>,
        fgfsm::row<states::replacing_values,  events::next, states::cumulating_values>,
        fgfsm::row<states::cumulating_values, events::next, states::idle>
    >;

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using context = ::context;
        using transition_table = ::transition_table;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("visit_event")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    REQUIRE(sm.is_active_state<states::idle>());

    sm.process_event(events::next{});
    REQUIRE(sm.is_active_state<states::replacing_values>());

    sm.process_event(events::int_data{42});
    REQUIRE(ctx.i == 42);

    sm.process_event(events::text_data{"42"});
    REQUIRE(ctx.str == "42");

    sm.process_event(events::next{});
    REQUIRE(sm.is_active_state<states::cumulating_values>());

    sm.process_event(events::int_data{1});
    REQUIRE(ctx.i == 43);

    sm.process_event(events::text_data{"1"});
    REQUIRE(ctx.str == "421");
}

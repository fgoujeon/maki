//Copyright Florian Goujeon 2021.
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
        std::string hello;
        std::string dog;
        std::string goodbye;
    };

    namespace events
    {
        struct say_dog{};
        struct next_language_request{};
    }

    namespace states
    {
        struct idle
        {
            context& ctx;
        };

        struct english
        {
            void on_entry()
            {
                ctx.hello = "hello";
            }

            void on_event(const fgfsm::event& event)
            {
                if(event.is<events::say_dog>())
                    ctx.dog = "dog";
            }

            void on_exit()
            {
                ctx.goodbye = "goodbye";
            }

            context& ctx;
        };

        struct french
        {
            void on_entry()
            {
                ctx.hello = "bonjour";
            }

            void on_event(const fgfsm::event& event)
            {
                if(event.is<events::say_dog>())
                    ctx.dog = "chien";
            }

            void on_exit()
            {
                ctx.goodbye = "au revoir";
            }

            context& ctx;
        };

        struct off
        {
            context& ctx;
        };
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::idle,    events::next_language_request, states::english>,
        fgfsm::row<states::english, events::next_language_request, states::french>,
        fgfsm::row<states::french,  events::next_language_request, states::idle>
    >;

    using fsm = fgfsm::fsm<transition_table>;
}

TEST_CASE("on_entry_event_exit")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    REQUIRE(sm.is_current_state<states::idle>());
    REQUIRE(ctx.hello == "");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_current_state<states::english>());
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::say_dog{});
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_current_state<states::french>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "goodbye");

    sm.process_event(events::say_dog{});
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "goodbye");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_current_state<states::idle>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "au revoir");
}

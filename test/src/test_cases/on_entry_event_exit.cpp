//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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
        struct idle{};

        struct english
        {
            void on_entry()
            {
                ctx.hello = "hello";
            }

            void on_event(const events::say_dog&)
            {
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

            void on_event(const events::say_dog&)
            {
                ctx.dog = "chien";
            }

            void on_exit()
            {
                ctx.goodbye = "au revoir";
            }

            context& ctx;
        };
    }

    struct sm_transition_table
    {
        using type = awesm::transition_table
        <
            awesm::row<states::idle,    events::next_language_request, states::english>,
            awesm::row<states::english, events::next_language_request, states::french>,
            awesm::row<states::french,  events::next_language_request, states::idle>
        >;
    };

    using sm_t = awesm::simple_sm<sm_transition_table>;
}

TEST_CASE("on_entry_event_exit")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    REQUIRE(sm.is_active_state<states::idle>());
    REQUIRE(ctx.hello == "");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_active_state<states::english>());
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::say_dog{});
    REQUIRE(sm.is_active_state<states::english>());
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_active_state<states::french>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "goodbye");

    sm.process_event(events::say_dog{});
    REQUIRE(sm.is_active_state<states::french>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "goodbye");

    sm.process_event(events::next_language_request{});
    REQUIRE(sm.is_active_state<states::idle>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "au revoir");
}

//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
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
        EMPTY_STATE(idle);

        constexpr auto english = maki::state_c
            .set_on_entry([](context& ctx)
            {
                ctx.hello = "hello";
            })
            .add_on_event(maki::type_c<events::say_dog>, [](context& ctx)
            {
                ctx.dog = "dog";
            })
            .set_on_exit([](context& ctx)
            {
                ctx.goodbye = "goodbye";
            })
        ;

        constexpr auto french = maki::state_c
            .set_on_entry([](context& ctx)
            {
                ctx.hello = "bonjour";
            })
            .add_on_event<events::say_dog>([](context& ctx)
            {
                ctx.dog = "chien";
            })
            .set_on_exit([](context& ctx)
            {
                ctx.goodbye = "au revoir";
            })
        ;
    }

    using transition_table_t = maki::transition_table
        ::add<states::idle,    events::next_language_request, states::english>
        ::add<states::english, events::next_language_request, states::french>
        ::add<states::french,  events::next_language_request, states::idle>
    ;

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("on_entry_event_exit")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    REQUIRE(machine.is_active_state(states::idle));
    REQUIRE(ctx.hello == "");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is_active_state(states::english));
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::say_dog{});
    REQUIRE(machine.is_active_state(states::english));
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is_active_state(states::french));
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "goodbye");

    machine.process_event(events::say_dog{});
    REQUIRE(machine.is_active_state(states::french));
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "goodbye");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is_active_state(states::idle));
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "au revoir");
}

//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace on_entry_event_exit_ns
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
        EMPTY_STATE(idle)

        constexpr auto english = maki::state_conf{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.hello = "hello";
                }
            )
            .internal_action_c<events::say_dog>
            (
                [](context& ctx)
                {
                    ctx.dog = "dog";
                }
            )
            .exit_action_c
            (
                [](context& ctx)
                {
                    ctx.goodbye = "goodbye";
                }
            )
        ;

        constexpr auto french = maki::state_conf{}
            .entry_action_m
            (
                [](auto& mach)
                {
                    mach.context().hello = "bonjour";
                }
            )
            .internal_action_c<events::say_dog>
            (
                [](context& ctx)
                {
                    ctx.dog = "chien";
                }
            )
            .exit_action_m
            (
                [](auto& mach)
                {
                    mach.context().goodbye = "au revoir";
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init,      states::idle)
        (states::idle,    states::english, maki::event<events::next_language_request>)
        (states::english, states::french,  maki::event<events::next_language_request>)
        (states::french,  states::idle,    maki::event<events::next_language_request>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("entry_action_event_exit")
{
    using namespace on_entry_event_exit_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    REQUIRE(machine.is<states::idle>());
    REQUIRE(ctx.hello == "");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is<states::english>());
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::say_dog{});
    REQUIRE(machine.is<states::english>());
    REQUIRE(ctx.hello == "hello");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is<states::french>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "dog");
    REQUIRE(ctx.goodbye == "goodbye");

    machine.process_event(events::say_dog{});
    REQUIRE(machine.is<states::french>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "goodbye");

    machine.process_event(events::next_language_request{});
    REQUIRE(machine.is<states::idle>());
    REQUIRE(ctx.hello == "bonjour");
    REQUIRE(ctx.dog == "chien");
    REQUIRE(ctx.goodbye == "au revoir");
}

//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
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

        struct english
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_entry,
                awesm::state_opts::on_event<events::say_dog>,
                awesm::state_opts::on_exit
            >;

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
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_entry,
                awesm::state_opts::on_event<events::say_dog>,
                awesm::state_opts::on_exit
            >;

            template<class Sm, class Event>
            void on_entry(Sm& mach, const Event& /*event*/)
            {
                mach.context().hello = "bonjour";
            }

            void on_event(const events::say_dog&)
            {
                ctx.dog = "chien";
            }

            template<class Sm, class Event>
            void on_exit(Sm& mach, const Event& /*event*/)
            {
                mach.context().goodbye = "au revoir";
            }

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::idle,    events::next_language_request, states::english>
        ::add<states::english, events::next_language_request, states::french>
        ::add<states::french,  events::next_language_request, states::idle>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf_tpl
        <
            awesm::subsm_opts::transition_tables<sm_transition_table>,
            awesm::subsm_opts::context<context>
        >;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("on_entry_event_exit")
{
    auto sm = sm_t{};
    auto& ctx = sm.context();

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

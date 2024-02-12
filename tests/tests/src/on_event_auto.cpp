//Copyright Florian Goujeon 2021 - 2024.
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
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
            std::string data;
        };

        struct alert_button_press
        {
        };
    }

    namespace states
    {
        struct off_data
        {
            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            void on_event(const events::button_press& event)
            {
                ctx.out += event.data + "2;";
            }

            context& ctx;
        };

        constexpr auto off = maki::state_conf{}
            .context_c(maki::type<off_data>)
            .internal_action_ce
            (
                maki::any,
                [](off_data& ctx, const auto& event)
                {
                    ctx.on_event(event);
                }
            )
        ;

        struct on_data
        {
            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            void on_event(const events::button_press& /*event*/)
            {
                ctx.out += "_";
            }

            void on_event(const events::alert_button_press& /*event*/)
            {
                ctx.out += "beep;";
            }

            context& ctx;
        };

        constexpr auto on = maki::state_conf{}
            .context_c(maki::type<on_data>)
            .internal_action_ce
            (
                maki::any,
                [](on_data& ctx, const auto& event)
                {
                    ctx.on_event(event);
                }
            )
        ;
    }

    constexpr auto make_transition_table()
    {
        return maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on)
        (states::on,  maki::type<events::button_press>, states::off)
        ;
    }

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(make_transition_table)
        .context_a(maki::type<context>)
        .event_action_ce
        (
            maki::type<events::button_press>,
            [](context& ctx, const events::button_press& event)
            {
                ctx.out += event.data + "1;";
            }
        )
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("on_event_auto")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::button_press{"a"});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(ctx.out == "a1;");

    ctx.out.clear();
    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.active_state<states::on>());
    REQUIRE(ctx.out == "beep;");
}

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
        struct off
        {
            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            void on_event(const events::button_press& event)
            {
                ctx.out += event.data + "2;";
            }

            static constexpr auto conf = maki::state_conf_c<off>
                .event_action_de<maki::any>
                (
                    [](off& self, const auto& event)
                    {
                        self.on_event(event);
                    }
                )
            ;

            context& ctx;
        };

        struct on
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

            static constexpr auto conf = maki::state_conf_c<on>
                .event_action_de<maki::any>
                (
                    [](on& self, const auto& event)
                    {
                        self.on_event(event);
                    }
                )
            ;

            context& ctx;
        };
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
            .event_action_ce<events::button_press>
            (
                [](context& ctx, const events::button_press& event)
                {
                    ctx.out += event.data + "1;";
                }
            )
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("on_event_auto")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::button_press{"a"});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.out == "a1;");

    ctx.out.clear();
    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is_active_state<states::on>());
    REQUIRE(ctx.out == "beep;");
}

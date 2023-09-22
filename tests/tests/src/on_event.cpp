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
        constexpr auto off = maki::state_c
            .set_on_event([](auto& mach, const auto& event)
            {
                if constexpr(std::is_same_v<std::decay_t<decltype(event)>, events::button_press>)
                {
                    mach.context().out += event.data + "2;";
                }
            })
        ;

        constexpr auto on = maki::state_c
            .set_on_event([](auto& mach, const auto& event)
            {
                if constexpr(std::is_same_v<std::decay_t<decltype(event)>, events::button_press>)
                {
                    mach.context().out += "_";
                }
                else if constexpr(std::is_same_v<std::decay_t<decltype(event)>, events::alert_button_press>)
                {
                    mach.context().out += "beep;";
                }
            })
        ;
    }

    using transition_table_t = maki::transition_table
        ::add<states::off, events::button_press, states::on>
        ::add<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        using conf = maki::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
            ::on_event<events::button_press>
        ;

        void on_event(const events::button_press& event)
        {
            ctx.out += event.data + "1;";
        }

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("on_event")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    machine.start();

    ctx.out.clear();
    machine.process_event(events::button_press{"a"});
    REQUIRE(machine.is_active_state(states::on));
    REQUIRE(ctx.out == "a1;");

    ctx.out.clear();
    machine.process_event(events::alert_button_press{});
    REQUIRE(machine.is_active_state(states::on));
    REQUIRE(ctx.out == "beep;");
}

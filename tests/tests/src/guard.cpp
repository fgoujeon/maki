//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace guard_ns
{
    struct context
    {
        bool has_power = false;
    };

    namespace states
    {
        EMPTY_STATE(on)
        EMPTY_STATE(off)
    }

    namespace events
    {
        struct button_press
        {
            bool hard = false;
        };
    }

    namespace guards
    {
        constexpr auto has_power = maki::guard_c([](context& ctx)
        {
            return ctx.has_power;
        });

        constexpr auto is_pressing_hard = maki::guard_e([](const events::button_press& event)
        {
            return event.hard;
        });

        constexpr auto always_false = maki::guard_v([]
        {
            return false;
        });
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on,  maki::null, guards::has_power)
        (states::on,  maki::type<events::button_press>, states::off, maki::null, guards::always_false)
        (states::on,  maki::type<events::button_press>, states::off, maki::null, guards::is_pressing_hard)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a(maki::type<context>)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("guard")
{
    using namespace guard_ns;

    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::off>());

    machine.context().has_power = true;
    machine.process_event(events::button_press{true});
    REQUIRE(machine.is<states::on>());
}

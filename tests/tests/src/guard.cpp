//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
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
        bool has_power(context& ctx)
        {
            return ctx.has_power;
        }

        bool is_pressing_hard(context& /*ctx*/, const events::button_press& event)
        {
            return event.hard;
        }

        bool always_false(context& /*ctx*/)
        {
            return false;
        }
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on,  maki::null_c, guards::has_power)
        (states::on,  maki::type<events::button_press>, states::off, maki::null_c, guards::always_false)
        (states::on,  maki::type<events::button_press>, states::off, maki::null_c, guards::is_pressing_hard)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("guard")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::off>());

    machine.context().has_power = true;
    machine.process_event(events::button_press{true});
    REQUIRE(machine.active_state<states::on>());
}

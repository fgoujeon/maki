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
        EMPTY_STATE(on);
        EMPTY_STATE(off);
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

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on,  maki::noop, guards::has_power>
        .add_c<states::on,  events::button_press, states::off, maki::noop, guards::always_false>
        .add_c<states::on,  events::button_press, states::off, maki::noop, guards::is_pressing_hard>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("guard")
{
    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());

    machine.context().has_power = true;
    machine.process_event(events::button_press{true});
    REQUIRE(machine.is_active_state<states::on>());
}

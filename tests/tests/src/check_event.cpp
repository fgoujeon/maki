//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace check_event_ns
{
    struct context
    {
        bool side_effect = false;
    };

    namespace events
    {
        struct button_press
        {
            bool hard = false;
        };

        struct other_button_press
        {
        };
    }

    namespace actions
    {
        constexpr auto side_effect = maki::action_c([](context& ctx)
        {
            return ctx.side_effect = true;
        });
    }

    namespace guards
    {
        constexpr auto is_pressing_hard = maki::guard_e([](const events::button_press& event)
        {
            return event.hard;
        });
    }

    namespace states
    {
        namespace on_ns
        {
            namespace states
            {
                EMPTY_STATE(subon)
            }

            constexpr auto transition_table = maki::transition_table{}
                (states::subon, maki::type<events::other_button_press>, maki::null, actions::side_effect)
            ;
        }

        constexpr auto on = maki::state_conf{}
            .transition_tables(on_ns::transition_table)
        ;

        EMPTY_STATE(off)
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::button_press>, states::on,  actions::side_effect, guards::is_pressing_hard)
        (states::on,  maki::type<events::button_press>, states::off, actions::side_effect)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("check_event")
{
    using namespace check_event_ns;

    auto machine = machine_t{};
    machine.start();

    auto& ctx = machine.context();

    REQUIRE(machine.is<states::off>());
    REQUIRE(!machine.check_event(events::button_press{})); // should fail guard
    REQUIRE(machine.check_event(events::button_press{true}));
    REQUIRE(machine.is<states::off>());
    REQUIRE(machine.is<states::off>());
    REQUIRE(!ctx.side_effect);

    REQUIRE(!machine.check_event(events::other_button_press{}));
    REQUIRE(!ctx.side_effect);

    machine.process_event(events::button_press{true});
    ctx.side_effect = false;

    REQUIRE(machine.is<states::on>());
    REQUIRE(machine.check_event(events::button_press{}));
    REQUIRE(machine.is<states::on>());
    REQUIRE(!ctx.side_effect);

    REQUIRE(machine.check_event(events::other_button_press{}));
    REQUIRE(!ctx.side_effect);
}

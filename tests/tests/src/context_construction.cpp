//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace
{
    struct machine_def;
    using machine_t = maki::machine<machine_def>;

    struct context
    {
        machine_t& machine;
        int i = 42;
    };

    struct on_context
    {
        machine_t& machine;
        context& parent;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(emitting_red)
        EMPTY_STATE(emitting_green)
        EMPTY_STATE(emitting_blue)

        constexpr auto on_transition_table = maki::transition_table{}
            .add_c<states::emitting_red,   events::color_button_press, states::emitting_green>
            .add_c<states::emitting_green, events::color_button_press, states::emitting_blue>
            .add_c<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        constexpr auto on = maki::submachine_conf{}
            .transition_tables(on_transition_table)
            .context<on_context>()
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        .add_c<states::off, events::power_button_press, states::on>
        .add_c<states::on,  events::power_button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf{}
            .transition_tables(transition_table)
            .context<context>()
        ;
    };
}

TEST_CASE("context_construction")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(&ctx.machine == &machine);
    REQUIRE(ctx.i == 42);
}

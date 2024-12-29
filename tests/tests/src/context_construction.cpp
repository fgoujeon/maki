//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace context_construction_ns
{
    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    using machine_ref_t = maki::machine_ref_e
    <
        events::power_button_press,
        events::color_button_press
    >;

    struct context
    {
        machine_ref_t machine;
        int i = 42;
    };

    struct on_context
    {
        context& parent;
        machine_ref_t machine;
    };

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(emitting_red)
        EMPTY_STATE(emitting_green)
        EMPTY_STATE(emitting_blue)

        constexpr auto on_transition_table = maki::transition_table{}
            (states::emitting_red,   maki::type<events::color_button_press>, states::emitting_green)
            (states::emitting_green, maki::type<events::color_button_press>, states::emitting_blue)
            (states::emitting_blue,  maki::type<events::color_button_press>, states::emitting_red)
        ;

        constexpr auto on = maki::state_conf{}
            .transition_tables(on_transition_table)
            .context_cm(maki::type<on_context>)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::type<events::power_button_press>, states::on)
        (states::on,  maki::type<events::power_button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_am(maki::type<context>)
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("context_construction")
{
    using namespace context_construction_ns;

    auto machine = machine_t{};
    auto& ctx = machine.context();

    REQUIRE(ctx.i == 42);
}

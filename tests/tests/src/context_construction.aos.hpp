//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace AOS(context_construction_ns)
{
    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

#if AOS_ASYNC
    using machine_ref_t = maki::any_async_machine_ref_e
    <
        boost::cobalt::promise,
        events::power_button_press,
        events::color_button_press
    >;
#else
    using machine_ref_t = maki::machine_ref_e
    <
        events::power_button_press,
        events::color_button_press
    >;
#endif

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
            (maki::ini,  states::emitting_red)
            (states::emitting_red,   states::emitting_green, maki::event<events::color_button_press>)
            (states::emitting_green, states::emitting_blue, maki::event<events::color_button_press>)
            (states::emitting_blue,  states::emitting_red, maki::event<events::color_button_press>)
        ;

        constexpr auto on = maki::state_mold{}
            .transition_tables(on_transition_table)
            .context_cm<on_context>()
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on, maki::event<events::power_button_press>)
        (states::on,  states::off, maki::event<events::power_button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_am<context>()
            AOS_ASYNC_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST(context_construction)
    {
        auto machine = machine_t{};
        auto& ctx = machine.context();

        REQUIRE(ctx.i == 42);

        AOS_RETURN;
    }
}

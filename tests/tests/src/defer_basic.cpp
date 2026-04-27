//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace defer_basic_ns
{
    struct context
    {
        bool button_pressed = false;
    };

    namespace events
    {
        struct end_of_init{};
        struct button_press{};
    }

    namespace states
    {
        constexpr auto initializing = maki::state_mold{}
            .defer<events::button_press>();

        constexpr auto running = maki::state_mold{}
            .internal_action_c<events::button_press>(
                [](context& ctx)
                {
                    ctx.button_pressed = true;
                });
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,            states::initializing)
        (states::initializing, states::running, maki::event<events::end_of_init>)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("defer_basic")
{
    using namespace defer_basic_ns;

    auto machine = machine_t{};

    machine.start();

    machine.process_event(events::button_press{});
    REQUIRE(!machine.context().button_pressed);

    machine.process_event(events::end_of_init{});
    REQUIRE(machine.context().button_pressed);
}

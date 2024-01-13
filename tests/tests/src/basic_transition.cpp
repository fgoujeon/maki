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
    };

    namespace states
    {
        EMPTY_STATE(on)
        EMPTY_STATE(off)
    }

    namespace events
    {
        struct button_press{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::event<events::button_press>, states::on)
        (states::on,  maki::event<events::button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
        .auto_start(false)
        .run_to_completion(false)
        .exception_action_me([](auto& /*mach*/, const std::exception_ptr& /*eptr*/){})
    ;

    using machine_t = maki::make_machine<machine_conf>;
}

TEST_CASE("basic_transition")
{
    auto machine = machine_t{};

    REQUIRE(!machine.running());

    machine.start();
    REQUIRE(machine.active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::on>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.active_state<states::off>());
}

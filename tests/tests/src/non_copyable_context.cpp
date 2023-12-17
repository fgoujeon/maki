//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <memory>

namespace
{
    struct context
    {
        std::unique_ptr<int> pi;
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

    constexpr auto transition_table_t = maki::transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf
            .transition_tables(transition_table_t)
            .context<context>()
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("non_copyable_context")
{
    auto machine = machine_t{};

    REQUIRE(machine.is_active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::on>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());
}

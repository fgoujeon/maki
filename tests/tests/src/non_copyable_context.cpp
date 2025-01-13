//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <memory>

namespace non_copyable_context_ns
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

    constexpr auto transition_table = maki::transition_table{}
        (states::off, maki::event<events::button_press>, states::on)
        (states::on,  maki::event<events::button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("non_copyable_context")
{
    using namespace non_copyable_context_ns;

    auto machine = machine_t{};

    REQUIRE(machine.is<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::on>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is<states::off>());
}

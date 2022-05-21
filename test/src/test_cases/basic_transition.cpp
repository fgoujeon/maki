//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct context
    {
    };

    namespace states
    {
        struct on{};
        struct off{};
    }

    namespace events
    {
        struct button_press{};
    }

    struct sm_configuration: awesm::sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off, events::button_press, states::on>,
            awesm::row<states::on,  events::button_press, states::off>
        >;

        static constexpr auto enable_run_to_completion = false;
    };

    using sm = awesm::sm<sm_configuration>;
}

TEST_CASE("basic transition")
{
    auto ctx = context{};
    auto machine = sm{ctx};

    REQUIRE(machine.is_active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::on>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());
}

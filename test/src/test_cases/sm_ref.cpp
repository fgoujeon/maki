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
        struct on_button_press{};
        struct off_button_press{};
    }

    struct sm_configuration: awesm::sm_configuration
    {
        using transition_table = awesm::transition_table
        <
            awesm::row<states::off, events::on_button_press,  states::on>,
            awesm::row<states::on,  events::off_button_press, states::off>
        >;

        static constexpr auto enable_run_to_completion = false;
    };

    using sm = awesm::sm<sm_configuration>;
}

TEST_CASE("sm_ref")
{
    using sm_ref =
        awesm::sm_ref<events::on_button_press, events::off_button_press>
    ;

    auto ctx = context{};
    auto machine = sm{ctx};
    auto pmachine_ref_temp = std::make_unique<sm_ref>(machine); //test ref of ref
    auto machine_ref = sm_ref{*pmachine_ref_temp};
    pmachine_ref_temp.reset();

    REQUIRE(machine.is_active_state<states::off>());

    machine_ref.process_event(events::on_button_press{});
    REQUIRE(machine.is_active_state<states::on>());

    machine_ref.process_event(events::off_button_press{});
    REQUIRE(machine.is_active_state<states::off>());
}

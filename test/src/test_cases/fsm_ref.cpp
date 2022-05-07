//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
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

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table_t = fgfsm::transition_table
        <
            fgfsm::row<states::off, events::on_button_press,  states::on>,
            fgfsm::row<states::on,  events::off_button_press, states::off>
        >;

        static constexpr auto enable_run_to_completion = false;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("fsm_ref")
{
    using fsm_ref =
        fgfsm::fsm_ref<events::on_button_press, events::off_button_press>
    ;

    auto ctx = context{};
    auto sm = fsm{ctx};
    auto psm_ref_temp = std::make_unique<fsm_ref>(sm); //test ref of ref
    auto sm_ref = fsm_ref{*psm_ref_temp};
    psm_ref_temp.reset();

    REQUIRE(sm.is_active_state<states::off>());

    sm_ref.process_event(events::on_button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm_ref.process_event(events::off_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

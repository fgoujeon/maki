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
        bool has_power = false;
    };

    namespace states
    {
        FGFSM_SIMPLE_STATE(on)
        FGFSM_SIMPLE_STATE(off)
    }

    namespace events
    {
        struct button_press{};
    }

    namespace guards
    {
        bool has_power(context& ctx, const fgfsm::any_cref&)
        {
            return ctx.has_power;
        }
    }

    using transition_table = fgfsm::transition_table
    <
        fgfsm::row<states::off, events::button_press, states::on,  fgfsm::none, fgfsm::fn<guards::has_power>>,
        fgfsm::row<states::on,  events::button_press, states::off, fgfsm::none>
    >;

    struct fsm_configuration: fgfsm::fsm_configuration
    {
        using transition_table = ::transition_table;
    };

    using fsm = fgfsm::fsm<fsm_configuration>;
}

TEST_CASE("guard")
{
    auto ctx = context{};
    auto sm = fsm{ctx};

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    ctx.has_power = true;
    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());
}

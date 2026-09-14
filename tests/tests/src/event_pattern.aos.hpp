//Copyright Florian Goujeon 2021 - 2026.
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
        struct power_button_press{};
        struct alert_button_press{};
    }

    [[maybe_unused]]
    constexpr auto make_sm_transition_table()
    {
        constexpr auto any_button_press =
            maki::event<events::power_button_press> ||
            maki::event<events::alert_button_press>
        ;

        return maki::transition_table{}
            (maki::ini,   states::off)
            (states::off, states::on, any_button_press)
            (states::on,  states::off, maki::event<events::power_button_press>)
        ;
    }

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(make_sm_transition_table())
            .context_a<context>()
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("event_set")
    {
        auto machine = machine_t{};

#if AOS_ASYNC
        co_await machine.async_start();
#endif

        REQUIRE(machine.is<states::off>());

        AOS_CALL machine.AOS(process_event)(events::power_button_press{});
        REQUIRE(machine.is<states::on>());

        AOS_CALL machine.AOS(process_event)(events::power_button_press{});
        REQUIRE(machine.is<states::off>());

        AOS_CALL machine.AOS(process_event)(events::alert_button_press{});
        REQUIRE(machine.is<states::on>());

        AOS_CALL machine.AOS(process_event)(events::alert_button_press{});
        REQUIRE(machine.is<states::on>());
    }
}

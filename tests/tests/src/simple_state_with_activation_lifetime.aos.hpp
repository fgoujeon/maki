//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <memory>

namespace AOS(simple_state_with_activation_lifetime)
{
    struct context
    {
        std::unique_ptr<int> pi;
    };

    struct on_context
    {
        on_context(context& parent):
            i(*parent.pi)
        {
        }

        int& i;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)

        constexpr auto on = maki::state_mold{}
            .context_c<on_context>()
            .context_lifetime(maki::state_context_lifetime::state_activity)
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("simple_state_with_activation_lifetime")
    {
        auto machine = machine_t{};

        AOS_CALL machine.AOS(start)();

        machine.context().pi = std::make_unique<int>(42);

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(machine.is<states::on>());
        REQUIRE(machine.state<states::on>().context().has_value());
        REQUIRE(machine.state<states::on>().context()->i == 42);

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(machine.is<states::off>());
        REQUIRE(!machine.state<states::on>().context().has_value());
    }
}

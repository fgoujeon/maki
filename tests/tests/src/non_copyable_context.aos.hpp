//Copyright Florian Goujeon 2021 - 2026.
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

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on, maki::event<events::button_press>)
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

    AOS_TEST_CASE("non_copyable_context")
    {
        auto machine = machine_t{};

#if AOS_ASYNC
        co_await machine.async_start();
#endif

        REQUIRE(machine.is<states::off>());

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(machine.is<states::on>());

        AOS_CALL machine.AOS(process_event)(events::button_press{});
        REQUIRE(machine.is<states::off>());
    }
}

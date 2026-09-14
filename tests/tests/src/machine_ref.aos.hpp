//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace AOS(machine_ref_ns)
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
        struct on_button_press{};
        struct off_button_press{};
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::on_button_press>)
        (states::on,  states::off, maki::event<events::off_button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .run_to_completion(false)
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("machine_ref")
    {
        using machine_ref_t =
#if AOS_ASYNC
            maki::any_async_machine_ref_e<boost::cobalt::promise, events::on_button_press, events::off_button_press>
#else
            maki::machine_ref_e<events::on_button_press, events::off_button_press>
#endif
        ;

        auto machine = machine_t{};
        auto pmachine_ref_temp = std::make_unique<machine_ref_t>(machine); //test ref of ref
        const auto machine_ref = machine_ref_t{*pmachine_ref_temp};
        pmachine_ref_temp.reset();

        AOS_CALL machine.AOS(start)();

        REQUIRE(machine.is<states::off>());

        AOS_CALL machine_ref.AOS(process_event)(events::on_button_press{});
        REQUIRE(machine.is<states::on>());

        AOS_CALL machine_ref.AOS(process_event)(events::off_button_press{});
        REQUIRE(machine.is<states::off>());
    }
}

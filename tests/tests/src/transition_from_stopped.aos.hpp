//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

namespace AOS(transition_from_stopped_any_state_ns)
{
    struct context{};

    struct some_event{};

    namespace states
    {
        EMPTY_STATE(idle)
        EMPTY_STATE(running)
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,        states::idle)
        (states::idle,     maki::null,      maki::null)
        (maki::all_states, states::running, maki::event<some_event>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .auto_start(false)
            AOS_MACHINE_OPTS
        ;
    };

    using machine_t = maki::machine<machine_conf>;

    AOS_TEST_CASE("transition_from_stopped_any_state")
    {
        auto machine = machine_t{};
        REQUIRE(!machine.running());

        /*
        We don't want to execute the transition from `all_states` to `running` in
        this case.
        */
        AOS_CALL machine.AOS(process_event)(some_event{});
        REQUIRE(!machine.running());
    }
}

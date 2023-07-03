//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
        std::unique_ptr<int> pi;
    };

    namespace states
    {
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    namespace events
    {
        struct button_press{};
    }

    using transition_table_t = awesm::transition_table
        ::add<states::off, events::button_press, states::on>
        ::add<states::on,  events::button_press, states::off>
    ;

    struct machine_def
    {
        using conf = awesm::machine_conf
            ::transition_tables<transition_table_t>
            ::context<context>
        ;
    };

    using machine_t = awesm::machine<machine_def>;
}

TEST_CASE("non_copyable_context")
{
    auto machine = machine_t{};

    REQUIRE(machine.is_active_state<states::off>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::on>());

    machine.process_event(events::button_press{});
    REQUIRE(machine.is_active_state<states::off>());
}

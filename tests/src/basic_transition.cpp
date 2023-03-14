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

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::button_press, states::on>
        ::add<states::on,  events::button_press, states::off>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            awesm::sm_opts::transition_tables<sm_transition_table>,
            awesm::sm_opts::context<context>,
            awesm::sm_opts::no_auto_start,
            awesm::sm_opts::no_run_to_completion,
            awesm::sm_opts::on_exception
        >;

        void on_exception(const std::exception_ptr& /*eptr*/)
        {
        }
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("basic_transition")
{
    auto sm = sm_t{};

    REQUIRE(!sm.is_running());

    sm.start();
    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

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
        struct on_button_press{};
        struct off_button_press{};
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::on_button_press,  states::on>
            .add<states::on,  events::off_button_press, states::off>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_opts::disable_run_to_completion,
            awesm::sm_opts::on_exception
        >;

        void on_exception(const std::exception_ptr& /*eptr*/)
        {
        }
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("sm_ref")
{
    using sm_ref_t =
        awesm::sm_ref<events::on_button_press, events::off_button_press>
    ;

    auto ctx = context{};
    auto sm = sm_t{ctx};
    auto psm_ref_temp = std::make_unique<sm_ref_t>(sm); //test ref of ref
    auto sm_ref = sm_ref_t{*psm_ref_temp};
    psm_ref_temp.reset();

    sm.start();

    REQUIRE(sm.is_active_state<states::off>());

    sm_ref.process_event(events::on_button_press{});
    REQUIRE(sm.is_active_state<states::on>());

    sm_ref.process_event(events::off_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

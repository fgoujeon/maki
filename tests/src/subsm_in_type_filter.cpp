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

    namespace events
    {
        struct button_press{};
        struct off_button_press{};
        struct destruction_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(s0_sub);
        EMPTY_STATE(s1);

        auto s0_transition_table()
        {
            return awesm::transition_table
                .add<s0_sub, events::button_press, awesm::null>
            ;
        }

        struct s0
        {
            using conf = awesm::sm_conf
                ::transition_tables<s0_transition_table>
            ;

            template<class Event>
            void on_entry(const Event& /*event*/)
            {
            }

            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            template<class Event>
            void on_exit(const Event& /*event*/)
            {
            }
        };
    }

    using any_but_s0_s1 = awesm::any_but<states::s0, states::s1>;
    using any = awesm::any_of<states::s0, states::s1>;

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off,   events::button_press,             states::s0>
            .add<states::s0,    events::button_press,             states::s1>
            .add<any_but_s0_s1, events::off_button_press,         states::off>
            .add<any,           events::destruction_button_press, states::off>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
            ::transition_tables<sm_transition_table>
            ::context<context>
        ;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("subsm_in_type_filter")
{
    auto sm = sm_t{};

    sm.start();
    REQUIRE(sm.is_active_state<states::off>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::s0>());

    sm.process_event(events::off_button_press{});
    REQUIRE(sm.is_active_state<states::s0>());

    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::s1>());

    sm.process_event(events::off_button_press{});
    REQUIRE(sm.is_active_state<states::s1>());

    sm.process_event(events::destruction_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
}

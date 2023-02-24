//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"
#include <string>

namespace
{
    namespace events
    {
        struct button_press
        {
            int pressure = 0;
        };
    }

    struct context
    {
        std::string out;
    };

    namespace states
    {
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::button_press, states::on>
            .add<states::on,  events::button_press, states::off>
        ;
    }

    struct sm_def;

    using sm_t = awesm::sm<sm_def>;

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_opts::before_state_transition,
            awesm::sm_opts::after_state_transition,
            awesm::sm_opts::disable_auto_start,
            awesm::sm_opts::get_pretty_name
        >;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& event)
        {
            static_assert(std::is_same_v<RegionPath, awesm::make_region_path<sm_t, 0>>);

            ctx.out += "Transition in ";
            ctx.out += RegionPath::get_pretty_name();
            ctx.out += ": ";
            ctx.out += awesm::get_pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += awesm::get_pretty_name<TargetState>();
            ctx.out += "...;";

            ctx.out += std::to_string(event.pressure) + ";";
        }

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& event)
        {
            static_assert(std::is_same_v<RegionPath, awesm::make_region_path<sm_t, 0>>);

            ctx.out += std::to_string(event.pressure) + ";";

            ctx.out += "Transition in ";
            ctx.out += RegionPath::get_pretty_name();
            ctx.out += ": ";
            ctx.out += awesm::get_pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += awesm::get_pretty_name<TargetState>();
            ctx.out += ";";
        }

        static constexpr auto get_pretty_name()
        {
            return "main_sm";
        }

        context& ctx;
    };
}

TEST_CASE("state_transition_hook_set")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    sm.start(events::button_press{0});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm: stopped -> off...;"
        "0;0;"
        "Transition in main_sm: stopped -> off;"
    );

    ctx.out.clear();
    sm.process_event(events::button_press{1});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm: off -> on...;"
        "1;1;"
        "Transition in main_sm: off -> on;"
    );

    ctx.out.clear();
    sm.process_event(events::button_press{2});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm: on -> off...;"
        "2;2;"
        "Transition in main_sm: on -> off;"
    );
}

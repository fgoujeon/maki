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
        EMPTY_STATE(off0);
        EMPTY_STATE(off1);
        EMPTY_STATE(on0);

        using on1_transition_table = awesm::transition_table
            ::add<states::off0, events::button_press, states::on0>
        ;

        struct on1
        {
            using conf = awesm::sm_conf_tpl
            <
                awesm::sm_opts::transition_tables<on1_transition_table>,
                awesm::sm_opts::get_pretty_name
            >;

            static auto get_pretty_name()
            {
                return "on_1";
            }
        };
    }

    using sm_transition_table_0 = awesm::transition_table
        ::add<states::off0, events::button_press, states::on0>
    ;

    using sm_transition_table_1 = awesm::transition_table
        ::add<states::off1, events::button_press, states::on1>
    ;

    struct sm_def
    {
        using conf = awesm::root_sm_conf
            ::transition_tables<sm_transition_table_0, sm_transition_table_1>
            ::context<context>
            ::before_state_transition
            ::after_state_transition
            ::no_auto_start
            ::get_pretty_name
        ;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& event)
        {
            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
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
            ctx.out += std::to_string(event.pressure) + ";";

            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
            ctx.out += ": ";
            ctx.out += awesm::get_pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += awesm::get_pretty_name<TargetState>();
            ctx.out += ";";
        }

        static auto get_pretty_name()
        {
            return "main_sm";
        }

        context& ctx;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("state_transition_hook_set")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    using root_0_path = awesm::region_path<sm_def, 0>;
    using root_1_path = awesm::region_path<sm_def, 1>;
    using root_1_on_1_path = root_1_path::add<states::on1>;

    sm.start(events::button_press{0});
    REQUIRE(sm.is_active_state<root_0_path, states::off0>());
    REQUIRE(sm.is_active_state<root_1_path, states::off1>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm[0]: stopped -> off0...;0;"
        "0;Transition in main_sm[0]: stopped -> off0;"
        "Transition in main_sm[1]: stopped -> off1...;0;"
        "0;Transition in main_sm[1]: stopped -> off1;"
    );

    ctx.out.clear();
    sm.process_event(events::button_press{1});
    REQUIRE(sm.is_active_state<root_0_path, states::on0>());
    REQUIRE(sm.is_active_state<root_1_path, states::on1>());
    REQUIRE(sm.is_active_state<root_1_on_1_path, states::off0>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm[0]: off0 -> on0...;1;"
        "1;Transition in main_sm[0]: off0 -> on0;"
        "Transition in main_sm[1]: off1 -> on_1...;1;"
        "Transition in main_sm[1].on_1: stopped -> off0...;1;"
        "1;Transition in main_sm[1].on_1: stopped -> off0;"
        "1;Transition in main_sm[1]: off1 -> on_1;"
    );

    ctx.out.clear();
    sm.process_event(events::button_press{2});
    REQUIRE(sm.is_active_state<root_0_path, states::on0>());
    REQUIRE(sm.is_active_state<root_1_path, states::on1>());
    REQUIRE(sm.is_active_state<root_1_on_1_path, states::on0>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm[1].on_1: off0 -> on0...;2;"
        "2;Transition in main_sm[1].on_1: off0 -> on0;"
    );
}

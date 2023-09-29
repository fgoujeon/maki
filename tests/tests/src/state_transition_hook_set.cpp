//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
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

        using on1_transition_table = maki::transition_table
            ::add<states::off0, events::button_press, states::on0>
        ;

        struct on1
        {
            static constexpr auto conf = maki::submachine_conf_c
                .set_transition_tables<on1_transition_table>()
                .pretty_name_fn()
            ;

            static auto pretty_name()
            {
                return "on_1";
            }
        };
    }

    using transition_table_0_t = maki::transition_table
        ::add<states::off0, events::button_press, states::on0>
    ;

    using transition_table_1_t = maki::transition_table
        ::add<states::off1, events::button_press, states::on1>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_0_t, transition_table_1_t>()
            .set_context_type<context>()
            .before_state_transition()
            .after_state_transition()
            .disable_auto_start()
            .pretty_name_fn()
        ;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& event)
        {
            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
            ctx.out += ": ";
            ctx.out += maki::pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += maki::pretty_name<TargetState>();
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
            ctx.out += maki::pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += maki::pretty_name<TargetState>();
            ctx.out += ";";
        }

        static auto pretty_name()
        {
            return "main_sm";
        }

        context& ctx;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state_transition_hook_set")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    using root_0_path = maki::region_path<machine_def, 0>;
    using root_1_path = maki::region_path<machine_def, 1>;
    using root_1_on_1_path = root_1_path::add<states::on1>;

    machine.start(events::button_press{0});
    REQUIRE(machine.is_active_state<root_0_path, states::off0>());
    REQUIRE(machine.is_active_state<root_1_path, states::off1>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm[0]: stopped -> off0...;0;"
        "0;Transition in main_sm[0]: stopped -> off0;"
        "Transition in main_sm[1]: stopped -> off1...;0;"
        "0;Transition in main_sm[1]: stopped -> off1;"
    );

    ctx.out.clear();
    machine.process_event(events::button_press{1});
    REQUIRE(machine.is_active_state<root_0_path, states::on0>());
    REQUIRE(machine.is_active_state<root_1_path, states::on1>());
    REQUIRE(machine.is_active_state<root_1_on_1_path, states::off0>());
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
    machine.process_event(events::button_press{2});
    REQUIRE(machine.is_active_state<root_0_path, states::on0>());
    REQUIRE(machine.is_active_state<root_1_path, states::on1>());
    REQUIRE(machine.is_active_state<root_1_on_1_path, states::on0>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm[1].on_1: off0 -> on0...;2;"
        "2;Transition in main_sm[1].on_1: off0 -> on0;"
    );
}

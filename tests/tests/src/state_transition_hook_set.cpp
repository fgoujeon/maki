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
        EMPTY_STATE(off0)
        EMPTY_STATE(off1)
        EMPTY_STATE(on0)

        constexpr auto on1_transition_table = maki::empty_transition_table
            .add_c<states::off0, events::button_press, states::on0>
        ;

        constexpr auto on1 = maki::submachine_conf
            .transition_tables(on1_transition_table)
            .pretty_name("on_1")
        ;
    }

    constexpr auto transition_table_0 = maki::empty_transition_table
        .add_c<states::off0, events::button_press, states::on0>
    ;

    constexpr auto transition_table_1 = maki::empty_transition_table
        .add_c<states::off1, events::button_press, states::on1>
    ;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf
            .transition_tables(transition_table_0, transition_table_1)
            .context<context>()
            .pre_state_transition_action_crset
            (
                [](context& ctx, const auto& region_path_constant, const auto source_state_constant, const auto& event, const auto target_state_constant)
                {
                    ctx.out += "Transition in ";
                    ctx.out += region_path_constant.value.to_string();
                    ctx.out += ": ";
                    ctx.out += maki::pretty_name<source_state_constant.value>();
                    ctx.out += " -> ";
                    ctx.out += maki::pretty_name<target_state_constant.value>();
                    ctx.out += "...;";

                    ctx.out += std::to_string(event.pressure) + ";";
                }
            )
            .post_state_transition_action_crset
            (
                [](context& ctx, const auto& region_path_constant, const auto source_state_constant, const auto& event, const auto target_state_constant)
                {
                    ctx.out += std::to_string(event.pressure) + ";";

                    ctx.out += "Transition in ";
                    ctx.out += region_path_constant.value.to_string();
                    ctx.out += ": ";
                    ctx.out += maki::pretty_name<source_state_constant.value>();
                    ctx.out += " -> ";
                    ctx.out += maki::pretty_name<target_state_constant.value>();
                    ctx.out += ";";
                }
            )
            .auto_start(false)
            .pretty_name("main_sm")
        ;
    };

    using machine_t = maki::machine<machine_def>;
}

TEST_CASE("state_transition_hook_set")
{
    auto machine = machine_t{};
    auto& ctx = machine.context();

    static constexpr auto root_0_path = maki::region_path_c<machine_def::conf, 0>;
    static constexpr auto root_1_path = maki::region_path_c<machine_def::conf, 1>;
    static constexpr auto root_1_on_1_path = root_1_path.add<states::on1>();

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

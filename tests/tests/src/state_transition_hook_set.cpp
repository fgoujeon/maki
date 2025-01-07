//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace state_transition_hook_set
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

        constexpr auto on1_transition_table = maki::transition_table{}
            (states::off0, maki::type<events::button_press>, states::on0)
        ;

        constexpr auto on1 = maki::state_conf{}
            .transition_tables(on1_transition_table)
            .pretty_name("on_1")
        ;
    }

    constexpr auto transition_table_0 = maki::transition_table{}
        (states::off0, maki::type<events::button_press>, states::on0)
    ;

    constexpr auto transition_table_1 = maki::transition_table{}
        (states::off1, maki::type<events::button_press>, states::on1)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table_0, transition_table_1)
        .context_a<context>()
        .pre_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto source_state_constant, const auto& event, const auto target_state_constant)
            {
                ctx.out += "Transition in ";
                ctx.out += region_path.to_string();
                ctx.out += ": ";
                ctx.out += maki::pretty_name<source_state_constant.value>();
                ctx.out += " -> ";
                ctx.out += maki::pretty_name<target_state_constant.value>();
                ctx.out += "...;";

                ctx.out += std::to_string(event.pressure) + ";";
            }
        )
        .post_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto source_state_constant, const auto& event, const auto target_state_constant)
            {
                ctx.out += std::to_string(event.pressure) + ";";

                ctx.out += "Transition in ";
                ctx.out += region_path.to_string();
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

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("state_transition_hook_set")
{
    using namespace state_transition_hook_set;

    auto machine = machine_t{};
    auto& ctx = machine.context();
    const auto& region0 = machine.region<0>();
    const auto& region1 = machine.region<1>();
    const auto state_on1 = region1.state<states::on1>();

    machine.start(events::button_press{0});
    REQUIRE(region0.is<states::off0>());
    REQUIRE(region1.is<states::off1>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm/0: stopped -> off0...;0;"
        "0;Transition in main_sm/0: stopped -> off0;"
        "Transition in main_sm/1: stopped -> off1...;0;"
        "0;Transition in main_sm/1: stopped -> off1;"
    );

    ctx.out.clear();
    machine.process_event(events::button_press{1});
    REQUIRE(region0.is<states::on0>());
    REQUIRE(region1.is<states::on1>());
    REQUIRE(state_on1.is<states::off0>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm/0: off0 -> on0...;1;"
        "1;Transition in main_sm/0: off0 -> on0;"
        "Transition in main_sm/1: off1 -> on_1...;1;"
        "Transition in main_sm/1/on_1/0: stopped -> off0...;1;"
        "1;Transition in main_sm/1/on_1/0: stopped -> off0;"
        "1;Transition in main_sm/1: off1 -> on_1;"
    );

    ctx.out.clear();
    machine.process_event(events::button_press{2});
    REQUIRE(region0.is<states::on0>());
    REQUIRE(region1.is<states::on1>());
    REQUIRE(state_on1.is<states::on0>());
    REQUIRE
    (
        ctx.out ==
        "Transition in main_sm/1/on_1/0: off0 -> on0...;2;"
        "2;Transition in main_sm/1/on_1/0: off0 -> on0;"
    );
}

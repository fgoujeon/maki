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
        std::string& out;
    };

    namespace states
    {
        EMPTY_STATE(on)
        EMPTY_STATE(off)
    }

    constexpr auto transition_table = maki::empty_transition_table
        .add_c<states::off, events::button_press, states::on>
        .add_c<states::on,  events::button_press, states::off>
    ;

    struct machine_def;

    using machine_t = maki::machine<machine_def>;

    struct machine_def
    {
        static constexpr auto conf = maki::default_machine_conf
            .set_transition_tables(transition_table)
            .set_context<context>()
            .pre_state_transition_action_crset
            (
                [](context& ctx, const auto& region_path_constant, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
                {
                    static_assert(region_path_constant.value == maki::region_path<maki::region_path_element<machine_def, 0>>{});

                    ctx.out += "Transition in ";
                    ctx.out += region_path_constant.value.to_string();
                    ctx.out += ": ";
                    ctx.out += maki::pretty_name<source_state_constant.value>();
                    ctx.out += " -> ";
                    ctx.out += maki::pretty_name<target_state_constant.value>();
                    ctx.out += "...;";
                }
            )
            .post_state_transition_action_crset
            (
                [](context& ctx, const auto& region_path_constant, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
                {
                    static_assert(region_path_constant.value == maki::region_path<maki::region_path_element<machine_def, 0>>{});

                    ctx.out += "Transition in ";
                    ctx.out += region_path_constant.value.to_string();
                    ctx.out += ": ";
                    ctx.out += maki::pretty_name<source_state_constant.value>();
                    ctx.out += " -> ";
                    ctx.out += maki::pretty_name<target_state_constant.value>();
                    ctx.out += ";";
                }
            )
            .pretty_name("main_sm")
        ;

        context& ctx;
    };
}

TEST_CASE("double_start_stop")
{
    auto out = std::string{};
    auto machine = machine_t{out};

    machine.start();
    machine.start();

    REQUIRE(machine.is_active_state<states::off>());
    REQUIRE
    (
        out ==
        "Transition in main_sm: stopped -> off...;"
        "Transition in main_sm: stopped -> off;"
    );

    out.clear();
    machine.stop();
    REQUIRE(!machine.is_running());
    REQUIRE
    (
        out ==
        "Transition in main_sm: off -> stopped...;"
        "Transition in main_sm: off -> stopped;"
    );

    out.clear();
    machine.stop();
    REQUIRE(!machine.is_running());
    REQUIRE(out == "");
}

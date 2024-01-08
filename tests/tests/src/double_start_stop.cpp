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

    constexpr auto transition_table = maki::transition_table{}
        .add<states::off, events::button_press, states::on>()
        .add<states::on,  events::button_press, states::off>()
    ;

    struct machine_conf_holder;

    using machine_t = maki::machine<machine_conf_holder>;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context<context>()
        .pre_state_transition_action_crset
        (
            [](context& ctx, const auto& path_constant, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
            {
                //REQUIRE(path_constant.value == maki::path<maki::path_element<machine_def, 0>>{});

                ctx.out += "Transition in ";
                ctx.out += maki::to_string(path_constant);
                ctx.out += ": ";
                ctx.out += maki::pretty_name<source_state_constant.value>();
                ctx.out += " -> ";
                ctx.out += maki::pretty_name<target_state_constant.value>();
                ctx.out += "...;";
            }
        )
        .post_state_transition_action_crset
        (
            [](context& ctx, const auto& path_constant, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
            {
                //REQUIRE(path_constant.value == maki::path<maki::path_element<machine_def, 0>>{});

                ctx.out += "Transition in ";
                ctx.out += maki::to_string(path_constant);
                ctx.out += ": ";
                ctx.out += maki::pretty_name<source_state_constant.value>();
                ctx.out += " -> ";
                ctx.out += maki::pretty_name<target_state_constant.value>();
                ctx.out += ";";
            }
        )
        .pretty_name("main_sm")
    ;

    struct machine_conf_holder: maki::conf_holder<machine_conf>{};
}

TEST_CASE("double_start_stop")
{
    auto out = std::string{};
    auto machine = machine_t{out};

    machine.start();
    machine.start();

    REQUIRE(machine.active_state<states::off>());
    REQUIRE
    (
        out ==
        "Transition in main_sm/0: stopped -> off...;"
        "Transition in main_sm/0: stopped -> off;"
    );

    out.clear();
    machine.stop();
    REQUIRE(!machine.running());
    REQUIRE
    (
        out ==
        "Transition in main_sm/0: off -> stopped...;"
        "Transition in main_sm/0: off -> stopped;"
    );

    out.clear();
    machine.stop();
    REQUIRE(!machine.running());
    REQUIRE(out == "");
}

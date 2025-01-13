//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace double_start_stop_ns
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
        (states::off, maki::event<events::button_press>, states::on)
        (states::on,  maki::event<events::button_press>, states::off)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .pre_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
            {
                //REQUIRE(path_constant.value == maki::path<maki::path_element<machine_def, 0>>{});

                ctx.out += "Transition in ";
                ctx.out += region_path.to_string();
                ctx.out += ": ";
                ctx.out += maki::pretty_name<source_state_constant.value>();
                ctx.out += " -> ";
                ctx.out += maki::pretty_name<target_state_constant.value>();
                ctx.out += "...;";
            }
        )
        .post_state_transition_hook_crset
        (
            [](context& ctx, const auto& region_path, const auto source_state_constant, const auto& /*event*/, const auto target_state_constant)
            {
                //REQUIRE(path_constant.value == maki::path<maki::path_element<machine_def, 0>>{});

                ctx.out += "Transition in ";
                ctx.out += region_path.to_string();
                ctx.out += ": ";
                ctx.out += maki::pretty_name<source_state_constant.value>();
                ctx.out += " -> ";
                ctx.out += maki::pretty_name<target_state_constant.value>();
                ctx.out += ";";
            }
        )
        .pretty_name("main_sm")
    ;

    using machine_t = maki::machine<machine_conf>;
}

TEST_CASE("double_start_stop")
{
    using namespace double_start_stop_ns;

    auto out = std::string{};
    auto machine = machine_t{out};

    machine.start();
    machine.start();

    REQUIRE(machine.is<states::off>());
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

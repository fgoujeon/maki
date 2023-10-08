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
        EMPTY_STATE(on);
        EMPTY_STATE(off);
    }

    constexpr auto transition_table = maki::transition_table_c
        .add<states::off, events::button_press, states::on>
        .add<states::on,  events::button_press, states::off>
    ;

    struct machine_def;

    using machine_t = maki::machine<machine_def>;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables(transition_table)
            .set_context_type<context>()
            .enable_before_state_transition()
            .enable_after_state_transition()
            .enable_pretty_name()
        ;

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& /*event*/)
        {
            static_assert(RegionPath == maki::region_path<maki::region_path_element<machine_def, 0>>{});

            ctx.out += "Transition in ";
            ctx.out += RegionPath.to_string();
            ctx.out += ": ";
            ctx.out += maki::pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += maki::pretty_name<TargetState>();
            ctx.out += "...;";
        }

        template<const auto& RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& /*event*/)
        {
            static_assert(RegionPath == maki::region_path<maki::region_path_element<machine_def, 0>>{});

            ctx.out += "Transition in ";
            ctx.out += RegionPath.to_string();
            ctx.out += ": ";
            ctx.out += maki::pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += maki::pretty_name<TargetState>();
            ctx.out += ";";
        }

        static constexpr auto pretty_name()
        {
            return "main_sm";
        }

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

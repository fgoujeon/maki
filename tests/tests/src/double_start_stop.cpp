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

    using transition_table_t = maki::transition_table
        ::add<states::off, events::button_press, states::on>
        ::add<states::on,  events::button_press, states::off>
    ;

    struct machine_def;

    using machine_t = maki::machine<machine_def>;

    struct machine_def
    {
        static constexpr auto conf = maki::machine_conf_c
            .set_transition_tables<transition_table_t>()
            .set_context_type<context>()
            .before_state_transition()
            .after_state_transition()
            .pretty_name_fn()
        ;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& /*event*/)
        {
            static_assert(std::is_same_v<RegionPath, maki::region_path<machine_def>>);

            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
            ctx.out += ": ";
            ctx.out += maki::pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += maki::pretty_name<TargetState>();
            ctx.out += "...;";
        }

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& /*event*/)
        {
            static_assert(std::is_same_v<RegionPath, maki::region_path<machine_def>>);

            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
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

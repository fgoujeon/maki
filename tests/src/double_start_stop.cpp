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
        std::string& out;
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
            ::transition_table<sm_transition_table>
            ::context<context>
            ::before_state_transition<true>
            ::after_state_transition<true>
            ::get_pretty_name<true>
        ;

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void before_state_transition(const Event& /*event*/)
        {
            static_assert(std::is_same_v<RegionPath, awesm::make_region_path<sm_def>>);

            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
            ctx.out += ": ";
            ctx.out += awesm::get_pretty_name<SourceState>();
            ctx.out += " -> ";
            ctx.out += awesm::get_pretty_name<TargetState>();
            ctx.out += "...;";
        }

        template<class RegionPath, class SourceState, class Event, class TargetState>
        void after_state_transition(const Event& /*event*/)
        {
            static_assert(std::is_same_v<RegionPath, awesm::make_region_path<sm_def>>);

            ctx.out += "Transition in ";
            ctx.out += RegionPath::to_string();
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

TEST_CASE("double_start_stop")
{
    auto out = std::string{};
    auto sm = sm_t{out};

    sm.start();
    sm.start();

    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE
    (
        out ==
        "Transition in main_sm: stopped -> off...;"
        "Transition in main_sm: stopped -> off;"
    );

    out.clear();
    sm.stop();
    REQUIRE(!sm.is_running());
    REQUIRE
    (
        out ==
        "Transition in main_sm: off -> stopped...;"
        "Transition in main_sm: off -> stopped;"
    );

    out.clear();
    sm.stop();
    REQUIRE(!sm.is_running());
    REQUIRE(out == "");
}

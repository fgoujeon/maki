//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct button_push{};
    }

    namespace states
    {
        struct off
        {
            void on_entry()
            {
                ctx.out += "off::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "off::on_exit;";
            }

            context& ctx;
        };

        struct on
        {
            void on_entry()
            {
                ctx.out += "on::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "on::on_exit;";
            }

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::off, events::button_push, states::on>,
        awesm::row<states::on,  events::button_push, states::off>
    >;

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            awesm::transition_table_list<sm_transition_table>,
            awesm::sm_options::before_entry
        >;

        template<int RegionIndex, class SourceState, class Event, class TargetState>
        void before_entry(const Event& /*event*/)
        {
            if constexpr(std::is_same_v<TargetState, states::off>)
            {
                ctx.out += "off::before_entry;";
            }
            else if constexpr(std::is_same_v<TargetState, states::on>)
            {
                ctx.out += "on::before_entry;";
            }
        }

        context& ctx;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("before_entry")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.out == "off::before_entry;off::on_entry;");

    ctx.out.clear();
    sm.process_event(events::button_push{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.out == "off::on_exit;on::before_entry;on::on_entry;");

    ctx.out.clear();
    sm.process_event(events::button_push{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.out == "on::on_exit;off::before_entry;off::on_entry;");
}

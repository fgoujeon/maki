//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"
#include <string>

namespace
{
    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct power_button_press{};
        struct beep_button_press{};
    }

    namespace states
    {
        struct idle
        {
            using conf_type = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_event_any,
                awesm::state_options::on_exit_any
            >;

            void on_entry()
            {
                ctx.out += "idle::on_entry;";
            }

            template<class Event>
            void on_event(const Event&)
            {
                ctx.out += "idle::on_event;";
            }

            void on_exit()
            {
                ctx.out += "idle::on_exit;";
            }

            context& ctx;
        };

        struct running
        {
            using conf_type = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_event_any,
                awesm::state_options::on_exit_any
            >;

            void on_entry()
            {
                ctx.out += "running::on_entry;";
            }

            template<class Event>
            void on_event(const Event&)
            {
                ctx.out += "running::on_event;";
            }

            void on_exit()
            {
                ctx.out += "running::on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        void beep(context& ctx)
        {
            ctx.out += "beep;";
        }
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::idle,    events::power_button_press, states::running>,
        awesm::row<states::running, events::power_button_press, states::idle>,
        awesm::row<states::running, events::beep_button_press,  awesm::null,      actions::beep>
    >;

    struct sm_def
    {
        using conf_type = awesm::sm_conf<sm_transition_table, context>;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("internal_transition_in_transition_table")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(ctx.out == "idle::on_entry;");

    ctx.out.clear();
    sm.process_event(events::power_button_press{});
    REQUIRE(ctx.out == "idle::on_event;idle::on_exit;running::on_entry;");

    ctx.out.clear();
    sm.process_event(events::beep_button_press{});
    REQUIRE(ctx.out == "running::on_event;beep;");
}

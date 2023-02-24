//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct context
    {
        bool exception_thrown = false;
        std::string out;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        struct off
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any,
                awesm::state_opts::on_exit_any
            >;

            void on_entry()
            {
                ctx.out += "off::on_entry;";
            }

            void on_exit()
            {
                ctx.out += "off::on_exit;";

                if(!ctx.exception_thrown)
                {
                    ctx.exception_thrown = true;
                    throw std::runtime_error{"error"};
                }
            }

            context& ctx;
        };

        struct on
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any,
                awesm::state_opts::on_exit_any
            >;

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

    namespace actions
    {
        void unreachable(context& ctx)
        {
            ctx.out += "unreachable;";
        }
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::button_press, states::on>
            .add<states::off, events::button_press, states::on,  actions::unreachable>
            .add<states::on,  events::button_press, states::off, actions::unreachable>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context
        >;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("basic_exception")
{
    auto sm = sm_t{};

    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(sm.get_context().out == "off::on_entry;");

    sm.get_context().out.clear();
    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(sm.get_context().out == "off::on_exit;");
}

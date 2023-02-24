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
    struct context
    {
        int always_zero = 0;
        std::string out;
    };

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
            }

            context& ctx;
        };

        struct on
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any,
                awesm::state_opts::on_event_any_of<awesm::events::exception>,
                awesm::state_opts::on_exit_any
            >;

            void on_entry()
            {
                ctx.out += "on::on_entry;";

                if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                {
                    throw std::runtime_error{"test;"};
                }
            }

            void on_event(const awesm::events::exception& event)
            {
                try
                {
                    std::rethrow_exception(event.eptr);
                }
                catch(const std::exception& e)
                {
                    ctx.out += "default;";
                    ctx.out += e.what();
                }
            }

            void on_exit()
            {
                ctx.out += "on::on_exit;";
            }

            context& ctx;
        };
    }

    namespace events
    {
        struct button_press{};
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::button_press, states::on>
            .add<states::on,  events::button_press, states::off>
        ;
    }

    struct default_sm_def
    {
        using conf = awesm::sm_conf<sm_transition_table, context>;
    };

    using default_sm_t = awesm::sm<default_sm_def>;

    struct custom_sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_opts::on_exception
        >;

        void on_exception(const std::exception_ptr& eptr)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch(const std::exception& e)
            {
                ctx.out += "custom;";
                ctx.out += e.what();
            }
        }

        context& ctx;
    };

    using custom_sm_t = awesm::sm<custom_sm_def>;
}

TEST_CASE("on_exception")
{
    {
        auto sm = default_sm_t{};
        auto& ctx = sm.get_context();

        sm.start();
        ctx.out.clear();

        sm.process_event(events::button_press{});
        REQUIRE(sm.is_active_state<states::on>());
        REQUIRE(ctx.out == "off::on_exit;on::on_entry;default;test;");
    }

    {
        auto sm = custom_sm_t{};
        auto& ctx = sm.get_context();

        sm.start();
        ctx.out.clear();

        sm.process_event(events::button_press{});
        REQUIRE(sm.is_active_state<states::on>());
        REQUIRE(ctx.out == "off::on_exit;on::on_entry;custom;test;");
    }
}

//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"

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
            void on_entry(awesm::whatever)
            {
                ctx.out += "off::on_entry;";
            }

            void on_event(awesm::whatever)
            {
            }

            void on_exit(awesm::whatever)
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
            void on_entry(awesm::whatever)
            {
                ctx.out += "on::on_entry;";
            }

            void on_event(awesm::whatever)
            {
            }

            void on_exit(awesm::whatever)
            {
                ctx.out += "on::on_exit;";
            }

            context& ctx;
        };
    }

    namespace actions
    {
        struct unreachable
        {
            void execute()
            {
                ctx.out += "unreachable;";
            }

            context& ctx;
        };
    }

    struct sm_transition_table
    {
        using type = awesm::transition_table
        <
            awesm::row<states::off, events::button_press, states::on>,
            awesm::row<states::off, events::button_press, states::on,  actions::unreachable>,
            awesm::row<states::on,  events::button_press, states::off, actions::unreachable>
        >;
    };

    using sm_t = awesm::simple_sm<sm_transition_table>;
}

TEST_CASE("basic_exception")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.out == "off::on_entry;");

    ctx.out.clear();
    sm.process_event(events::button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.out == "off::on_exit;");
}

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
        std::string out;
    };

    namespace events
    {
        struct button_press
        {
            std::string data;
        };
    }

    namespace states
    {
        struct off
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_event_any_of<events::button_press>
            >;

            void on_event(const events::button_press& event)
            {
                ctx.out += event.data + "2;";
            }

            context& ctx;
        };

        struct on
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_event_any_of<events::button_press>
            >;

            void on_event(const events::button_press& /*event*/)
            {
                ctx.out += "_";
            }

            context& ctx;
        };
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
            .add<states::off, events::button_press, states::on>
            .add<states::on,  events::button_press, states::off>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            context,
            awesm::sm_opts::on_event
        >;

        template<class Event>
        void on_event(const Event& /*event*/)
        {
        }

        void on_event(const events::button_press& event)
        {
            ctx.out += event.data + "1;";
        }

        context& ctx;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("on_event")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    sm.start();
    ctx.out.clear();

    sm.process_event(events::button_press{"a"});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.out == "a1;");
}

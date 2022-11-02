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
        struct button_press
        {
            std::string data;
        };
    }

    namespace states
    {
        struct off
        {
            void on_entry()
            {
            }

            void on_event(const events::button_press& event)
            {
                ctx.out += event.data + "2;";
            }

            void on_exit()
            {
            }

            context& ctx;
        };

        struct on
        {
            void on_entry()
            {
            }

            void on_event(const events::button_press& /*event*/)
            {
                ctx.out += "_";
            }

            void on_exit()
            {
            }

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
    <
        awesm::row<states::off, events::button_press, states::on>,
        awesm::row<states::on,  events::button_press, states::off>
    >;

    struct sm_def
    {
        using conf = awesm::sm_conf
        <
            sm_transition_table,
            awesm::sm_options::on_event
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
    auto ctx = context{};

    auto sm = sm_t{ctx};
    sm.start();
    ctx.out.clear();

    sm.process_event(events::button_press{"a"});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.out == "a1;a2;");
}
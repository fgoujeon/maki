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
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

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

        struct internal
        {
            std::string data;
        };
    }

    namespace states
    {
        EMPTY_STATE(off);

        struct on_0
        {
            using conf = awesm::state_conf
            <
                awesm::state_options::on_entry_any,
                awesm::state_options::on_event_any_of<events::internal>,
                awesm::state_options::on_exit_any
            >;

            void on_entry(const events::button_press& event)
            {
                ctx.out += event.data + "2";
            }

            void on_event(const events::internal& event)
            {
                ctx.out += event.data + "1";
            }

            void on_exit(const events::button_press& event)
            {
                ctx.out += event.data + "1";
            }

            context& ctx;
        };

        using on_transition_table = awesm::transition_table
        <
            awesm::row<states::on_0, events::button_press, void>
        >;

        struct on
        {
            using conf = awesm::composite_state_conf
            <
                on_transition_table
            >;

            void on_entry(const events::button_press& event)
            {
                ctx.out += event.data + "1";
            }

            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            void on_event(const events::internal& event)
            {
                ctx.out += event.data + "2";
            }

            void on_exit(const events::button_press& event)
            {
                ctx.out += event.data + "2";
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
        using conf = awesm::sm_conf<sm_transition_table>;
    };
}

TEST_CASE("composite_state_on_xxx")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    sm.process_event(events::button_press{"a"});
    REQUIRE(ctx.out == "a1a2");

    ctx.out.clear();
    sm.process_event(events::internal{"b"});
    REQUIRE(ctx.out == "b1b2");

    ctx.out.clear();
    sm.process_event(events::button_press{"c"});
    REQUIRE(ctx.out == "c1c2");
}

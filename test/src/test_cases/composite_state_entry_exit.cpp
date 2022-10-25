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
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        struct off{};

        struct on_0
        {
            void on_entry()
            {
                ctx.out += "2";
            }

            void on_exit()
            {
                ctx.out += "1";
            }

            context& ctx;
        };

        using on_transition_table = awesm::transition_table
        <
            awesm::row<states::on_0, events::button_press, awesm::none>
        >;

        struct on_def
        {
            using transition_tables = awesm::transition_table_list<on_transition_table>;

            template<class Event>
            void on_entry(const Event& /*event*/)
            {
                ctx.out += "1";
            }

            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            template<class Event>
            void on_exit(const Event& /*event*/)
            {
                ctx.out += "2";
            }

            context& ctx;
        };

        using on = awesm::composite_state<on_def>;
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

TEST_CASE("composite_state_entry_exit")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    sm.process_event(events::button_press{});
    REQUIRE(ctx.out == "12");

    ctx.out.clear();
    sm.process_event(events::button_press{});
    REQUIRE(ctx.out == "12");
}

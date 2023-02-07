//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    struct sm_def;
    using sm_t = awesm::sm<sm_def>;

    enum class led_color
    {
        off,
        red,
        green,
        blue
    };

    struct context
    {
        led_color current_led_color = led_color::off;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        struct off
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::off;
            }

            context& ctx;
        };

        struct emitting_red
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::red;
            }

            context& ctx;
        };

        struct emitting_green
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::green;
            }

            context& ctx;
        };

        struct emitting_blue
        {
            using conf = awesm::state_conf
            <
                awesm::state_opts::on_entry_any
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::blue;
            }

            context& ctx;
        };

        auto on_transition_table()
        {
            return awesm::transition_table
            <
                awesm::row<states::emitting_red,   events::color_button_press, states::emitting_green>,
                awesm::row<states::emitting_green, events::color_button_press, states::emitting_blue>,
                awesm::row<states::emitting_blue,  events::color_button_press, states::emitting_red>
            >;
        }

        struct on
        {
            using conf = awesm::subsm_conf
            <
                on_transition_table
            >;

            template<class Event>
            void on_entry(const Event& /*event*/)
            {
            }

            template<class Event>
            void on_event(const Event& /*event*/)
            {
            }

            template<class Event>
            void on_exit(const Event& /*event*/)
            {
            }

            context& ctx;
        };
    }

    auto sm_transition_table()
    {
        return awesm::transition_table
        <
            awesm::row<states::on, events::power_button_press, states::off>
        >;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf<sm_transition_table, context>;
    };
}

TEST_CASE("initial_subsm")
{
    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.is_active_state<awesm::make_region_path<sm_t>::add<states::on>, states::emitting_red>());
    REQUIRE(ctx.current_led_color == led_color::red);

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::green);

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::blue);

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.current_led_color == led_color::off);
}

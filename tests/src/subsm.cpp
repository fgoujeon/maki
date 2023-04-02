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
        EMPTY_STATE(off);

        struct emitting_red
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_entry<>
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::red;
            }

            sm_t& sm;
            context& ctx;
        };

        struct emitting_green
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_entry<>
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::green;
            }

            sm_t& sm;
            context& ctx;
        };

        struct emitting_blue
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_entry<>
            >;

            void on_entry()
            {
                ctx.current_led_color = led_color::blue;
            }

            sm_t& sm;
            context& ctx;
        };

        using on_transition_table = awesm::transition_table
            ::add<states::emitting_red,   events::color_button_press, states::emitting_green>
            ::add<states::emitting_green, events::color_button_press, states::emitting_blue>
            ::add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            using conf = awesm::subsm_conf_tpl
            <
                awesm::subsm_opts::transition_tables<on_transition_table>,
                awesm::subsm_opts::on_exit<>
            >;

            void on_exit()
            {
                ctx.current_led_color = led_color::off;
            }

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::off, events::power_button_press, states::on>
        ::add<states::on,  events::power_button_press, states::off>
    ;

    struct sm_def
    {
        using conf = awesm::sm_conf_tpl
        <
            awesm::sm_opts::transition_tables<sm_transition_table>,
            awesm::sm_opts::context<context>
        >;
    };
}

TEST_CASE("subsm")
{
    using sm_on_region_path_t = awesm::region_path<sm_def>::add<states::on>;

    auto sm = sm_t{};
    auto& ctx = sm.context();

    sm.start();

    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(!sm.is_running<sm_on_region_path_t>());
    REQUIRE(ctx.current_led_color == led_color::off);

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_red>());
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

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(ctx.current_led_color == led_color::red);
}

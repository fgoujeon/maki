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
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);
        EMPTY_STATE(emitting_red);
        EMPTY_STATE(emitting_green);
        EMPTY_STATE(emitting_blue);

        using not_emitting_red = awesm::any_but<emitting_red>;
        using emitting_red_or_green = awesm::any_of<emitting_red, emitting_green>;

        auto on_transition_table()
        {
            return awesm::transition_table
                .add<states::emitting_red,   events::color_button_press, states::emitting_green>
                .add<states::emitting_green, events::color_button_press, states::emitting_blue>
                .add<states::emitting_blue,  events::color_button_press, states::emitting_red>
            ;
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
            .add<states::off, events::power_button_press, states::on>
            .add<states::on,  events::power_button_press, states::off>
        ;
    }

    struct sm_def
    {
        using conf = awesm::sm_conf<sm_transition_table, context>;
    };
}

TEST_CASE("is_active_state_pattern")
{
    using sm_on_region_path_t = awesm::make_region_path<sm_t>::add<states::on>;

    auto ctx = context{};
    auto sm = sm_t{ctx};

    sm.start();

    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(!sm.is_running<sm_on_region_path_t>());

    sm.process_event(events::power_button_press{});
    REQUIRE(!sm.is_active_state<states::emitting_red_or_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_red>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(!sm.is_active_state<sm_on_region_path_t, states::not_emitting_red>());

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::not_emitting_red>());

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::emitting_blue>());
    REQUIRE(!sm.is_active_state<sm_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::not_emitting_red>());

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(!sm.is_active_state<states::emitting_red_or_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, awesm::states::stopped>());
    REQUIRE(!sm.is_active_state<sm_on_region_path_t, states::emitting_red_or_green>());
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::not_emitting_red>());

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
}

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
        void clear()
        {
            ignored_event = "";
        }

        std::string ignored_event;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};

        struct ignored_by_emitting_blue
        {
            int value = 0;
        };
    }

    namespace states
    {
        struct off
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_event<events::ignored_by_emitting_blue>
            >;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_red
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_event_auto
            >;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_green
        {
            using conf = awesm::state_conf_tpl
            <
                awesm::state_opts::on_event_auto
            >;

            void on_event(const events::ignored_by_emitting_blue & /*unused*/)
            {
            }

            context& ctx;
        };

        struct emitting_blue
        {
            using conf = awesm::state_conf_tpl<>;
        };

        using on_transition_table = awesm::transition_table
            ::add<states::emitting_red,   events::color_button_press, states::emitting_green>
            ::add<states::emitting_green, events::color_button_press, states::emitting_blue>
            ::add<states::emitting_blue,  events::color_button_press, states::emitting_red>
        ;

        struct on
        {
            using conf = awesm::sm_conf_tpl
            <
                awesm::sm_opts::transition_tables<on_transition_table>
            >;

            context& ctx;
        };
    }

    using sm_transition_table = awesm::transition_table
        ::add<states::on, events::power_button_press, states::off>
    ;

    struct sm_def
    {
        using conf = awesm::root_sm_conf_tpl
        <
            awesm::root_sm_opts::transition_tables<sm_transition_table>,
            awesm::root_sm_opts::context<context>,
            awesm::root_sm_opts::on_unprocessed
        >;

        template<class T>
        void on_unprocessed(const T& /*unused*/)
        {
            ctx.ignored_event = "other";
        }

        void on_unprocessed(const events::ignored_by_emitting_blue& event)
        {
            ctx.ignored_event = "ignored_by_emitting_blue{" + std::to_string(event.value) + "}";
        }

        context& ctx;
    };
}

TEST_CASE("on_unprocessed")
{
    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    ctx.clear();
    sm.start();
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.is_active_state<awesm::region_path<sm_def>::add<states::on>, states::emitting_red>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    sm.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.is_active_state<awesm::region_path<sm_def>::add<states::on>, states::emitting_green>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    sm.process_event(events::ignored_by_emitting_blue{});
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
    REQUIRE(sm.is_active_state<awesm::region_path<sm_def>::add<states::on>, states::emitting_blue>());
    REQUIRE(ctx.ignored_event.empty());

    ctx.clear();
    sm.process_event(events::ignored_by_emitting_blue{42});
    REQUIRE(ctx.ignored_event == "ignored_by_emitting_blue{42}");

    ctx.clear();
    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::off>());
    REQUIRE(ctx.ignored_event.empty());
}

//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common.hpp"

namespace
{
    enum class led_color
    {
        off,
        red,
        green,
        blue
    };

    struct context
    {
        std::string out;
    };

    namespace events
    {
        struct power_button_press{};
        struct color_button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off);

        namespace on_ns
        {
            struct context
            {
                using parent_context_type = ::context;

                context(parent_context_type& parent):
                    parent(parent)
                {
                }

                parent_context_type& parent;
                int red_count = 0;
            };

            namespace emitting_red_ns
            {
                struct emitting_cold_red
                {
                    using conf = awesm::state_conf;
                    on_ns::context& ctx;
                };

                EMPTY_STATE(emitting_hot_red);

                auto make_transition_table()
                {
                    return awesm::transition_table
                        .add<emitting_cold_red, events::color_button_press, emitting_hot_red>
                    ;
                }
            }

            struct emitting_red
            {
                using conf = awesm::sm_conf
                    ::transition_table<emitting_red_ns::make_transition_table>
                    ::on_entry<true>
                ;

                void on_entry()
                {
                    ++ctx.red_count;
                }

                context& ctx;
            };

            EMPTY_STATE(emitting_green);
            EMPTY_STATE(emitting_blue);

            auto make_transition_table()
            {
                return awesm::transition_table
                    .add<emitting_red,   events::color_button_press, emitting_green>
                    .add<emitting_green, events::color_button_press, emitting_blue>
                    .add<emitting_blue,  events::color_button_press, emitting_red>
                ;
            }
        }

        struct on
        {
            using conf = awesm::sm_conf
                ::transition_table<on_ns::make_transition_table>
                ::context<on_ns::context>
                ::on_exit<true>
            ;

            void on_exit()
            {
                ctx.parent.out = std::to_string(ctx.red_count);
            }

            on_ns::context& ctx;
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
        using conf = awesm::sm_conf
            ::transition_table<sm_transition_table>
            ::context<context>
        ;
    };

    using sm_t = awesm::sm<sm_def>;
}

TEST_CASE("subsm_context")
{
    using sm_on_region_path_t = awesm::make_region_path<sm_def>::add<states::on>;

    auto sm = sm_t{};
    auto& ctx = sm.get_context();

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::on_ns::emitting_red>());

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::on_ns::emitting_green>());

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::on_ns::emitting_blue>());

    sm.process_event(events::color_button_press{});
    REQUIRE(sm.is_active_state<sm_on_region_path_t, states::on_ns::emitting_red>());

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::off>());

    REQUIRE(ctx.out == "2");

    sm.process_event(events::power_button_press{});
    REQUIRE(sm.is_active_state<states::on>());
}

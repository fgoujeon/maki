//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "comp_firewall_on.hpp"
#include "comp_firewall_common.hpp"
#include "common.hpp"
#include <string>

namespace comp_firewall_ns
{
    namespace machine_ns::on_ns
    {
        struct context
        {
            machine_ns::context& parent;
        };

        constexpr auto emitting_red = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.parent.current_led_color = led_color::red;
                }
            )
        ;

        constexpr auto emitting_green = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.parent.current_led_color = led_color::green;
                }
            )
        ;

        constexpr auto emitting_blue = maki::state_mold{}
            .entry_action_c
            (
                [](context& ctx)
                {
                    ctx.parent.current_led_color = led_color::blue;
                }
            )
        ;

        constexpr auto transition_table = maki::transition_table{}
            (maki::ini,      emitting_red)
            (emitting_red,   emitting_green, maki::event<events::color_button_press>)
            (emitting_green, emitting_blue,  maki::event<events::color_button_press>)
            (emitting_blue,  emitting_red,   maki::event<events::color_button_press>)
        ;

        struct machine_conf
        {
            static constexpr auto value = maki::machine_conf{}
                .auto_start(false)
                .transition_tables(on_ns::transition_table)
                .context_a<on_ns::context>()
                .run_to_completion(false)
                .process_event_now_enabled(true)
            ;
        };

        using machine_t = maki::machine<on_ns::machine_conf>;

        struct forwarder::impl
        {
            impl(machine_ns::context& parent_ctx):
                machine(parent_ctx)
            {
            }

            on_ns::machine_t machine;
        };

        forwarder::forwarder(machine_ref_type /*mach*/, machine_ns::context& parent_ctx):
            pimpl_(std::make_unique<impl>(parent_ctx))
        {
        }

        forwarder::~forwarder() = default;

        void forwarder::enter(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::power_button_press& /*event*/)
        {
            pimpl_->machine.start();
        }

        bool forwarder::process_event(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::color_button_press& event)
        {
            return pimpl_->machine.process_event_now(event);
        }

        void forwarder::exit(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::power_button_press& /*event*/)
        {
            pimpl_->machine.stop();
        }
    }
}

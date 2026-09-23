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

        struct on_conf
        {
            static constexpr auto value = maki::firewalled_state_conf{}
                .context_a<on_ns::context>()
                .transition_tables(on_ns::transition_table)
            ;
        };

        using firewalled_on = maki::firewalled_state<on_conf>;

        struct firewall::impl
        {
            impl(machine_ns::context& parent_ctx):
                state(parent_ctx)
            {
            }

            on_ns::firewalled_on state;
        };

        firewall::firewall(machine_ref_type /*mach*/, machine_ns::context& parent_ctx):
            pimpl_(std::make_unique<impl>(parent_ctx))
        {
        }

        firewall::~firewall() = default;

        void firewall::enter(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::power_button_press& event)
        {
            maki::detail::impl_of(pimpl_->state).enter(pimpl_->state, pimpl_->state.context(), event);
        }

        bool firewall::process_event(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::color_button_press& event)
        {
            return maki::detail::impl_of(pimpl_->state).process_event(pimpl_->state, pimpl_->state.context(), event);
        }

        void firewall::exit(
            context_param_type /*parent_ctx*/,
            machine_ref_type /*mach*/,
            const events::power_button_press& event)
        {
            maki::detail::impl_of(pimpl_->state).exit(pimpl_->state, pimpl_->state.context(), event);
        }
    }
}

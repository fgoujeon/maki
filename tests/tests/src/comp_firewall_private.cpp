//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "comp_firewall_private.hpp"
#include "comp_firewall_common.hpp"
#include "common.hpp"
#include <string>

namespace comp_firewall_ns
{
    namespace on_ns
    {
        struct context
        {
            comp_firewall_ns::context& parent;
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
    }

    struct on_forwarder::impl
    {
        impl(context& parent_ctx):
            machine(parent_ctx)
        {
        }

        on_ns::machine_t machine;
    };

    constexpr auto private_on = maki::state_mold{}
        .context_c<on_ns::context>()
    ;

    on_forwarder::on_forwarder(machine_ref /*mach*/, comp_firewall_ns::context& parent_ctx):
        pimpl_(std::make_unique<impl>(parent_ctx))
    {
    }

    on_forwarder::~on_forwarder() = default;

    void on_forwarder::enter_2(const events::power_button_press& event)
    {
        pimpl_->machine.start();
    }

    bool on_forwarder::process_event_2(const events::color_button_press& event)
    {
        return pimpl_->machine.process_event_now(event);
    }

    void on_forwarder::exit_2(const events::power_button_press& event)
    {
        pimpl_->machine.stop();
    }
}

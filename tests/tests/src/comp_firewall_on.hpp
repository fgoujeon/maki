//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef COMP_FIREWALL_PRIVATE_HPP
#define COMP_FIREWALL_PRIVATE_HPP

#include "comp_firewall_common.hpp"
#include <maki.hpp>
#include <memory>

namespace comp_firewall_ns
{
    namespace machine_ns
    {
        namespace on_ns
        {
            class firewall
            {
            public:
                struct context_param_type
                {
                    template<class Parent>
                    context_param_type(Parent& parent):
                        current_led_color(parent.current_led_color)
                    {
                    }

                    led_color& current_led_color;
                };

                using machine_ref_type = maki::machine_ref_e<
                    events::power_button_press,
                    events::color_button_press>;

                static constexpr auto event_type_set =
                    maki::no_event ||
                    maki::event<events::color_button_press>;

                static constexpr auto deferrable_event_type_set = maki::no_event;

                firewall(machine_ref_type mach, machine_ns::context& parent_ctx);

                ~firewall();

                void enter(
                    context_param_type parent_ctx,
                    machine_ref_type mach,
                    const events::power_button_press& event);

                bool process_event(
                    context_param_type parent_ctx,
                    machine_ref_type mach,
                    const events::color_button_press& event);

                void exit(
                    context_param_type parent_ctx,
                    machine_ref_type mach,
                    const events::power_button_press& event);

            private:
                struct impl;
                std::unique_ptr<impl> pimpl_;
            };
        }

        constexpr auto on = maki::firewall_state_mold{}
            .firewall<on_ns::firewall>()
        ;
    }
}

#endif

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
    namespace on_ns
    {
        struct context;
    }

    class on_forwarder
    {
    public:
        using context_type = on_ns::context;

        using machine_ref = maki::machine_ref_e<
            events::power_button_press,
            events::color_button_press>;

        static constexpr auto event_type_set =
            maki::event<events::power_button_press> ||
            maki::event<events::color_button_press>;

        static constexpr auto deferrable_event_type_set = maki::no_event;

        on_forwarder();

        ~on_forwarder();

        on_ns::context& context();

        const on_ns::context& context() const;

        template<class ParentContext, class Event>
        void enter(
            const machine_ref mach,
            ParentContext& parent_ctx,
            const Event& event)
        {
            enter_2(mach, event);
        }

        template<class ParentContext, class Event>
        bool process_event(
            const machine_ref mach,
            ParentContext& parent_ctx,
            const Event& event)
        {
            return process_event_2(mach, event);
        }

        template<class ParentContext, class Event>
        void exit(
            const machine_ref mach,
            ParentContext& parent_ctx,
            const Event& event)
        {
            exit_2(mach, event);
        }

    private:
        void enter_2(
            const machine_ref mach,
            const events::power_button_press& event);

        bool process_event_2(
            const machine_ref mach,
            const events::power_button_press& event);

        bool process_event_2(
            const machine_ref mach,
            const events::color_button_press& event);

        void exit_2(
            const machine_ref mach,
            const events::power_button_press& event);

        std::unique_ptr<on_ns::context> pctx_;
    };
}

#endif

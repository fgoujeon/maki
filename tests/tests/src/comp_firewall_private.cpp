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
        struct context{};
    }

    constexpr auto private_on = maki::state_mold{}
        .context_c<on_ns::context>()
    ;

    on_forwarder::on_forwarder():
        pctx_(std::make_unique<on_ns::context>())
    {
    }

    on_forwarder::~on_forwarder() = default;

    on_ns::context& on_forwarder::context()
    {
        return *pctx_;
    }

    const on_ns::context& on_forwarder::context() const
    {
        return *pctx_;
    }

    void on_forwarder::enter_2(
        const machine_ref mach,
        const events::power_button_press& event)
    {
    }

    bool on_forwarder::process_event_2(
        const machine_ref mach,
        const events::power_button_press& event)
    {
        return true;
    }

    bool on_forwarder::process_event_2(
        const machine_ref mach,
        const events::color_button_press& event)
    {
        return true;
    }

    void on_forwarder::exit_2(
        const machine_ref mach,
        const events::power_button_press& event)
    {
    }
}

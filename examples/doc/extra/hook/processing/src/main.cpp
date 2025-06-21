//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

class request_server
{
public:
    void send_to(const int client_id, const std::string_view message)
    {
        std::cout << "[-> client #" << client_id << "] " << message << '\n';
    }
};

struct context
{
    request_server server;
};

struct alert_event
{
    std::string reason;
};

struct client_reboot_request
{
    int client_id = 0;
};

template<class... Args>
void log_warning(const Args&... args)
{
    (std::cout << ... << args) << '\n';
}

constexpr auto some_state = maki::state_mold{};

constexpr auto transition_table = maki::transition_table{}
    (maki::ini, some_state)
;

constexpr auto machine_conf = maki::machine_conf{}
    .context_a<context>()
    .transition_tables(transition_table)
//! [pre_processing_hook]
    // Always log alerts
    .pre_processing_hook_e<alert_event>([](const alert_event& evt)
    {
        log_warning("Alert! ", evt.reason);
    })
//! [pre_processing_hook]
//! [post_processing_hook]
    // Notify client when reboot request is rejected
    .post_processing_hook_mep<client_reboot_request>([](
        auto& mach,
        const client_reboot_request& event,
        const bool processed)
    {
        if (!processed)
        {
            mach.context().server.send_to(
                event.client_id,
                "Can't reboot right now");
        }
    })
//! [post_processing_hook]
//! [external-transition-hooks]
//! [external-transition-hooks]
;

using machine_t = maki::machine<machine_conf>;

int main()
{
    auto mach = machine_t{};
    mach.process_event(alert_event{"Aliens are coming!"});
    mach.process_event(client_reboot_request{});
}

//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include <iostream>

template<class... Args>
void log_info(const Args&... args)
{
    (std::cout << ... << args) << '\n';
}

struct context
{
};

constexpr auto initializing = maki::state_mold{};
constexpr auto running = maki::state_mold{};

constexpr auto transition_table = maki::transition_table{}
    (maki::ini,    initializing)
    (initializing, running)
;

//! [signature]
template
<
    class RegionImpl,
    class SourceStateImpl,
    class TargetStateImpl,
    class Event
>
void hook
(
    context& ctx,
    const maki::region<RegionImpl>& region,
    const maki::state<SourceStateImpl>& source_state,
    const maki::state<TargetStateImpl>& target_state,
    const Event& event
);
//! [signature]

template
<
    class RegionImpl,
    class SourceStateImpl,
    class TargetStateImpl,
    class Event
>
void hook
(
    context& /*ctx*/,
    const maki::region<RegionImpl>& /*region*/,
    const maki::state<SourceStateImpl>& /*source_state*/,
    const maki::state<TargetStateImpl>& /*target_state*/,
    const Event& /*event*/
)
{
}

constexpr auto machine_conf = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
//! [external-transition-hooks]
//! [pre]
    .pre_external_transition_hook_crste
    (
        [](context& /*ctx*/, const auto& region, const auto& source_state, const auto& target_state, const auto& /*event*/)
        {
            log_info
            (
                "Executing transition in ",
                region.path().to_string(),
                ": ",
                source_state.pretty_name(),
                " -> ",
                target_state.pretty_name(),
                "..."
            );
        }
    )
//! [pre]
//! [post]
    .post_external_transition_hook_crste
    (
        [](context& /*ctx*/, const auto& region, const auto& source_state, const auto& target_state, const auto& /*event*/)
        {
            log_info
            (
                "Executed transition in ",
                region.path().to_string(),
                ": ",
                source_state.pretty_name(),
                " -> ",
                target_state.pretty_name(),
                "."
            );
        }
    )
//! [post]
//! [external-transition-hooks]
;

using machine_t = maki::machine<machine_conf>;

//Another machine conf to check the signature exposed by `hook()`.
constexpr auto machine_conf_2 = maki::machine_conf{}
    .transition_tables(transition_table)
    .context_a<context>()
    .pre_external_transition_hook_crste
    (
        [](context& ctx, const auto& region, const auto& source_state, const auto& target_state, const auto& event)
        {
            hook(ctx, region, source_state, target_state, event);
        }
    )
    .post_external_transition_hook_crste
    (
        [](context& ctx, const auto& region, const auto& source_state, const auto& target_state, const auto& event)
        {
            hook(ctx, region, source_state, target_state, event);
        }
    )
;

using machine_2_t = maki::machine<machine_conf_2>;

int main()
{
    auto machine = machine_t{};
    machine.process_event(0);

    auto machine_2 = machine_2_t{};
    machine_2.process_event(0);
}

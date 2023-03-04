//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TRANSITION_TABLE_HPP
#define AWESM_TRANSITION_TABLE_HPP

namespace awesm
{

/*
Represents either:
- a null event (for anonymous transitions);
- a null target state (for internal transitions in transition table).
*/
struct null{};

inline constexpr void noop()
{
}

inline constexpr bool yes()
{
    return true;
}

template
<
    class SourceStateFilter,
    class EventFilter,
    class TargetState,
    const auto& Action = noop,
    const auto& Guard = yes
>
struct transition
{
    using source_state_type_filter = SourceStateFilter;
    using event_type_filter = EventFilter;
    using target_state_type = TargetState;

    static constexpr const auto& get_action()
    {
        return Action;
    }

    static constexpr const auto& get_guard()
    {
        return Guard;
    }
};

template<class... Transitions>
struct transition_table_t
{
    template
    <
        class SourceStateFilter,
        class EventFilter,
        class TargetState,
        const auto& Action = noop,
        const auto& Guard = yes
    >
    static constexpr transition_table_t
    <
        Transitions...,
        transition<SourceStateFilter, EventFilter, TargetState, Action, Guard>
    > add = {};
};

inline constexpr auto transition_table = transition_table_t<>{};

} //namespace

#endif

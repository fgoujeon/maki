//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TRANSITION_TABLE_HPP
#define AWESM_TRANSITION_TABLE_HPP

#include "null.hpp"
#include "whatever.hpp"
#include <type_traits>

namespace awesm
{

inline constexpr void noop()
{
}

inline constexpr bool yes()
{
    return true;
}

template
<
    class SourceStatePattern,
    class EventPattern,
    class TargetState,
    const auto& Action = noop,
    const auto& Guard = yes
>
struct transition
{
    using source_state_type_pattern = SourceStatePattern;
    using event_type_pattern = EventPattern;
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
        class SourceStatePattern,
        class EventPattern,
        class TargetState,
        const auto& Action = noop,
        const auto& Guard = yes
    >
    static constexpr transition_table_t
    <
        Transitions...,
        transition<SourceStatePattern, EventPattern, TargetState, Action, Guard>
    > add = {};
};

inline constexpr auto transition_table = transition_table_t<>{};

template<auto... Fns>
struct transition_table_list_t{};

template<auto... Fns>
inline constexpr auto transition_table_list = static_cast<const transition_table_list_t<Fns...>*>(nullptr);

} //namespace

#endif

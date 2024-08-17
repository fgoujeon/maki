//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_conf struct template
*/

#ifndef MAKI_MACHINE_CONF_FWD_HPP
#define MAKI_MACHINE_CONF_FWD_HPP

#include "detail/context_signature.hpp"
#include "detail/tuple.hpp"
#include "null.hpp"
#include <string_view>
#include <cstdlib>

namespace maki
{

inline constexpr auto machine_conf_default_small_event_max_align = 8;
inline constexpr auto machine_conf_default_small_event_max_size = 16;

namespace detail
{
    template
    <
        class Context = void,
        class EntryActionTuple = detail::tuple<>,
        class EventActionTuple = detail::tuple<>,
        class ExitActionTuple = detail::tuple<>,
        class ExceptionAction = null_t,
        class PreStateTransitionAction = null_t,
        class PostStateTransitionAction = null_t,
        class FallbackTransitionActionTuple = detail::tuple<>,
        class TransitionTableTuple = detail::tuple<>
    >
    struct machine_conf_option_set
    {
        using context_type = Context;
        using exception_action_type = ExceptionAction;
        using pre_state_transition_action_type = PreStateTransitionAction;
        using post_state_transition_action_type = PostStateTransitionAction;
        using fallback_transition_action_tuple_type = FallbackTransitionActionTuple;

        bool auto_start = true;
        context_signature context_sig = context_signature::v;
        EntryActionTuple entry_actions;
        EventActionTuple internal_actions;
        ExitActionTuple exit_actions;
        PostStateTransitionAction post_state_transition_action = null;
        PreStateTransitionAction pre_state_transition_action = null;
        ExceptionAction exception_action = null;
        FallbackTransitionActionTuple fallback_transition_actions;
        std::string_view pretty_name;
        bool run_to_completion = true;
        std::size_t small_event_max_align = machine_conf_default_small_event_max_align;
        std::size_t small_event_max_size = machine_conf_default_small_event_max_size;
        TransitionTableTuple transition_tables;
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
/**
@brief @ref machine configuration
*/
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet = detail::machine_conf_option_set<>>
#endif
class machine_conf;

} //namespace

#endif

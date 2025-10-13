//Copyright Florian Goujeon 2021 - 2025.
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

#include "mix.hpp"
#include "../context.hpp"
#include "../null.hpp"
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
        class PreProcessingHookTuple = mix<>,
        class ExceptionHandler = null_t,
        class PreExternalTransitionHook = null_t,
        class PostExternalTransitionHook = null_t,
        class PostProcessingHookTuple = mix<>,
        class TransitionTableTuple = mix<>
    >
    struct machine_conf_option_set
    {
        using context_type = Context;
        using exception_handler_type = ExceptionHandler;
        using pre_external_transition_hook_type = PreExternalTransitionHook;
        using post_external_transition_hook_type = PostExternalTransitionHook;
        using pre_processing_hook_tuple_type = PreProcessingHookTuple;
        using post_processing_hook_tuple_type = PostProcessingHookTuple;
        using internal_action_mix_type = mix<>;

        bool auto_start = true;
        machine_context_signature context_sig = machine_context_signature::a;
        PreProcessingHookTuple pre_processing_hooks;
        PostExternalTransitionHook post_external_transition_hook = null;
        PreExternalTransitionHook pre_external_transition_hook = null;
        ExceptionHandler exception_handler = null;
        PostProcessingHookTuple post_processing_hooks;
        bool process_event_now_enabled = false;
        bool run_to_completion = true;
        std::size_t small_event_max_align = machine_conf_default_small_event_max_align;
        std::size_t small_event_max_size = machine_conf_default_small_event_max_size;
        TransitionTableTuple transition_tables;

        static constexpr auto entry_actions = mix<>{};
        static constexpr auto exit_actions = mix<>{};
        static constexpr auto internal_actions = mix<>{};
    };
}

#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class OptionSet = detail::machine_conf_option_set<>>
#endif
class machine_conf;

} //namespace

#endif

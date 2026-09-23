// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_conf struct template
*/

#ifndef MAKI_MACHINE_CONF_IMPL_HPP
#define MAKI_MACHINE_CONF_IMPL_HPP

#include "../context.hpp"
#include "../null.hpp"
#include "mix.hpp"
#include "type_set.hpp"
#include <cstdlib>

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_BEGIN /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_auto_start = impl_.auto_start; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_type = \
        detail::type<typename Impl::context_type>; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_context_sig = \
        impl_.context_sig; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_processing_hooks = \
        impl_.pre_processing_hooks; \
    [[maybe_unused]] const auto \
        MAKI_DETAIL_ARG_post_external_transition_hook = \
            impl_.post_external_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_pre_external_transition_hook = \
        impl_.pre_external_transition_hook; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_exception_handler = \
        impl_.exception_handler; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_post_processing_hooks = \
        impl_.post_processing_hooks; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_process_event_now_enabled = \
        impl_.process_event_now_enabled; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_run_to_completion = \
        impl_.run_to_completion; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_align = \
        impl_.small_event_max_align; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_small_event_max_size = \
        impl_.small_event_max_size; \
    [[maybe_unused]] const auto MAKI_DETAIL_ARG_transition_tables = \
        impl_.transition_tables;

#define MAKI_DETAIL_MAKE_MACHINE_CONF_COPY_END( \
    template_name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    return template_name<detail::machine_conf_impl< \
        typename std::decay_t<decltype(MAKI_DETAIL_ARG_context_type)>::type, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_pre_processing_hooks)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_exception_handler)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_pre_external_transition_hook)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_post_external_transition_hook)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_post_processing_hooks)>, \
        std::decay_t<decltype(MAKI_DETAIL_ARG_transition_tables)>>>{ \
        MAKI_DETAIL_ARG_auto_start, \
        MAKI_DETAIL_ARG_context_sig, \
        MAKI_DETAIL_ARG_pre_processing_hooks, \
        MAKI_DETAIL_ARG_post_external_transition_hook, \
        MAKI_DETAIL_ARG_pre_external_transition_hook, \
        MAKI_DETAIL_ARG_exception_handler, \
        MAKI_DETAIL_ARG_post_processing_hooks, \
        MAKI_DETAIL_ARG_process_event_now_enabled, \
        MAKI_DETAIL_ARG_run_to_completion, \
        MAKI_DETAIL_ARG_small_event_max_align, \
        MAKI_DETAIL_ARG_small_event_max_size, \
        MAKI_DETAIL_ARG_transition_tables};

namespace maki
{

inline constexpr auto machine_conf_default_small_event_max_align = 8;
inline constexpr auto machine_conf_default_small_event_max_size = 16;

} // namespace maki

namespace maki::detail
{

template<
    class Context = void,
    class PreProcessingHookTuple = mix<>,
    class ExceptionHandler = null_t,
    class PreExternalTransitionHook = null_t,
    class PostExternalTransitionHook = null_t,
    class PostProcessingHookTuple = mix<>,
    class TransitionTableTuple = mix<>>
struct machine_conf_impl
{
    using context_type = Context;
    using exception_handler_type = ExceptionHandler;
    using pre_external_transition_hook_type = PreExternalTransitionHook;
    using post_external_transition_hook_type = PostExternalTransitionHook;
    using pre_processing_hook_tuple_type = PreProcessingHookTuple;
    using post_processing_hook_tuple_type = PostProcessingHookTuple;
    using internal_action_mix_type = mix<>;
    using deferred_event_type_set = empty_type_set_t;

    bool auto_start = true;
    machine_context_signature context_sig = machine_context_signature::a;
    PreProcessingHookTuple pre_processing_hooks;
    PostExternalTransitionHook post_external_transition_hook = null;
    PreExternalTransitionHook pre_external_transition_hook = null;
    ExceptionHandler exception_handler = null;
    PostProcessingHookTuple post_processing_hooks;
    bool process_event_now_enabled = false;
    bool run_to_completion = true;
    std::size_t small_event_max_align =
        machine_conf_default_small_event_max_align;
    std::size_t small_event_max_size =
        machine_conf_default_small_event_max_size;
    TransitionTableTuple transition_tables;

    static constexpr auto context_lifetime = state_context_lifetime::parent;
    static constexpr auto entry_actions = mix<>{};
    static constexpr auto exit_actions = mix<>{};
    static constexpr auto internal_actions = mix<>{};
};

} // namespace maki::detail

#endif

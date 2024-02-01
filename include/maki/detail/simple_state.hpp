//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "call_member.hpp"
#include "maybe_bool_util.hpp"
#include "event_action.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"

namespace maki::detail
{

/*
Implementation of a non-composite state
*/
template<const auto& Conf>
class simple_state
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using context_type = typename option_set_type::context_type;

    static constexpr auto context_sig = opts(Conf).context_sig;

    template
    <
        class Machine,
        class... Args,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::a, bool> = true
    >
    simple_state(Machine& /*mach*/, Args&&... args):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template
    <
        class Machine,
        class... Args,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::am, bool> = true
    >
    simple_state(Machine& mach, Args&&... args):
        ctx_{std::forward<Args>(args)..., mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::c, bool> = true
    >
    simple_state(Machine& /*mach*/, ParentContext& parent_ctx):
        ctx_{parent_ctx}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::cm, bool> = true
    >
    simple_state(Machine& mach, ParentContext& parent_ctx):
        ctx_{parent_ctx, mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::m, bool> = true
    >
    simple_state(Machine& mach, ParentContext& /*parent_ctx*/):
        ctx_{mach}
    {
    }

    template
    <
        class Machine,
        class ParentContext,
        auto ContextSignature = context_sig,
        std::enable_if_t<ContextSignature == context_signature::v, bool> = true
    >
    simple_state(Machine& /*mach*/, ParentContext& /*parent_ctx*/)
    {
    }

    auto& context()
    {
        static_assert(has_own_context);
        return ctx_;
    }

    const auto& context() const
    {
        static_assert(has_own_context);
        return ctx_;
    }

    template<class ParentContext>
    auto& context_or(ParentContext& parent_ctx)
    {
        if constexpr(has_own_context)
        {
            return ctx_;
        }
        else
        {
            return parent_ctx;
        }
    }

    template<class ParentContext>
    const auto& context_or(ParentContext& parent_ctx) const
    {
        if constexpr(has_own_context)
        {
            return ctx_;
        }
        else
        {
            return parent_ctx;
        }
    }

    template<class Machine, class Event>
    void call_entry_action(Machine& mach, const Event& event)
    {
        static_assert(has_own_context);

        if constexpr(!tlu::empty_v<entry_action_cref_constant_list>)
        {
            /*
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_cref_constant_list>
            (
                mach,
                ctx_,
                event
            );
        }
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<entry_action_cref_constant_list>)
        {
            /*
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_cref_constant_list>
            (
                mach,
                context_or(parent_ctx),
                event
            );
        }
    }

    template<class Machine, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, const Event& event, MaybeBool&... processed)
    {
        static_assert(has_own_context);

        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_cref_constant_list>);

        call_matching_event_action<internal_action_cref_constant_list>
        (
            mach,
            context(),
            event
        );

        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& parent_ctx, const Event& event, MaybeBool&... processed)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_cref_constant_list>);

        call_matching_event_action<internal_action_cref_constant_list>
        (
            mach,
            context_or(parent_ctx),
            event
        );

        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Event>
    void call_exit_action(Machine& mach, const Event& event)
    {
        static_assert(has_own_context);

        if constexpr(!tlu::empty_v<exit_action_cref_constant_list>)
        {
            /*
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_cref_constant_list>
            (
                mach,
                ctx_,
                event
            );
        }
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<exit_action_cref_constant_list>)
        {
            /*
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_cref_constant_list>
            (
                mach,
                context_or(parent_ctx),
                event
            );
        }
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return tlu::contains_if_v
        <
            internal_action_cref_constant_list,
            event_action_traits::for_event<Event>::template has_matching_event_filter
        >;
    }

    static constexpr const auto& conf = Conf;

private:
    static constexpr bool has_own_context = !std::is_same_v<context_type, null_t>;

    static constexpr auto entry_actions = opts(Conf).entry_actions;
    using entry_action_cref_constant_list = tuple_to_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = opts(Conf).internal_actions;
    using internal_action_cref_constant_list = tuple_to_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = opts(Conf).exit_actions;
    using exit_action_cref_constant_list = tuple_to_constant_list_t<exit_actions>;

    context_type ctx_;
};

} //namespace

#endif

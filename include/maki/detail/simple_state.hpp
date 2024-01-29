//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "call_member.hpp"
#include "machine_of.hpp"
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
template<const auto& Conf, class Parent>
class simple_state
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;
    using context_type = typename option_set_type::context_type;
    using machine_type = machine_of_t<simple_state>;

    template<class... Args>
    simple_state
    (
        context_signature_a_tag_t /*tag*/,
        machine_type& mach,
        Args&&... args
    ):
        ctx_{std::forward<Args>(args)...},
        mach_(mach)
    {
    }

    template<class... Args>
    simple_state
    (
        context_signature_am_tag_t /*tag*/,
        machine_type& mach,
        Args&&... args
    ):
        ctx_{std::forward<Args>(args)..., mach},
        mach_(mach)
    {
    }

    template<class ParentContext>
    simple_state
    (
        context_signature_auto_tag_t /*tag*/,
        ParentContext& parent_ctx,
        machine_type& mach
    ):
        simple_state
        {
            typename option_set_type::context_sig_tag_type{},
            parent_ctx,
            mach
        }
    {
    }

    template<class ParentContext>
    simple_state
    (
        context_signature_c_tag_t /*tag*/,
        ParentContext& parent_ctx,
        machine_type& mach
    ):
        ctx_{parent_ctx},
        mach_(mach)
    {
    }

    template<class ParentContext>
    simple_state
    (
        context_signature_cm_tag_t /*tag*/,
        ParentContext& parent_ctx,
        machine_type& mach
    ):
        ctx_{parent_ctx, mach},
        mach_(mach)
    {
    }

    template<class ParentContext>
    simple_state
    (
        context_signature_m_tag_t /*tag*/,
        ParentContext& /*parent_ctx*/,
        machine_type& mach
    ):
        ctx_{mach},
        mach_(mach)
    {
    }

    template<class ParentContext>
    simple_state
    (
        context_signature_v_tag_t /*tag*/,
        ParentContext& /*parent_ctx*/,
        machine_type& mach
    ):
        mach_(mach)
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

    template<class Context, class Event>
    void call_entry_action(Context& parent_ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<entry_action_cref_constant_list>)
        {
            /*
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_cref_constant_list>
            (
                mach_,
                context_or(parent_ctx),
                event
            );
        }
    }

    template<class Context, class Event, class... MaybeBool>
    void call_internal_action(Context& parent_ctx, const Event& event, MaybeBool&... processed)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_cref_constant_list>);

        call_matching_event_action<internal_action_cref_constant_list>
        (
            mach_,
            context_or(parent_ctx),
            event
        );

        maybe_bool_util::set_to_true(processed...);
    }

    template<class Context, class Event>
    void call_exit_action(Context& parent_ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<exit_action_cref_constant_list>)
        {
            /*
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_cref_constant_list>
            (
                mach_,
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
    machine_type& mach_;
};

} //namespace

#endif

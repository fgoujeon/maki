//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "context_holder.hpp"
#include "call_member.hpp"
#include "maybe_bool_util.hpp"
#include "event_action.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"

namespace maki::detail
{

template<const auto& Conf>
class simple_state_no_context
{
public:
    using option_set_type = std::decay_t<decltype(opts(Conf))>;

    static constexpr auto context_sig = opts(Conf).context_sig;

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& ctx, const Event& event)
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
                ctx,
                event
            );
        }
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& ctx, const Event& event, MaybeBool&... processed)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_cref_constant_list>);

        call_matching_event_action<internal_action_cref_constant_list>
        (
            mach,
            ctx,
            event
        );

        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& ctx, const Event& event)
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
                ctx,
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
    static constexpr auto entry_actions = opts(Conf).entry_actions;
    using entry_action_cref_constant_list = tuple_to_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = opts(Conf).internal_actions;
    using internal_action_cref_constant_list = tuple_to_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = opts(Conf).exit_actions;
    using exit_action_cref_constant_list = tuple_to_constant_list_t<exit_actions>;
};

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

    template<class... Args>
    simple_state(Args&... args):
        ctx_holder_(args...)
    {
    }

    auto& context()
    {
        static_assert(has_own_context);
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        static_assert(has_own_context);
        return ctx_holder_.get();
    }

    template<class ParentContext>
    auto& context_or(ParentContext& parent_ctx)
    {
        if constexpr(has_own_context)
        {
            return context();
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
            return context();
        }
        else
        {
            return parent_ctx;
        }
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        impl_.call_entry_action(mach, context_or(parent_ctx), event);
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& parent_ctx, const Event& event, MaybeBool&... processed)
    {
        impl_.call_internal_action(mach, context_or(parent_ctx), event, processed...);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& parent_ctx, const Event& event)
    {
        impl_.call_exit_action(mach, context_or(parent_ctx), event);
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return impl_type::template has_internal_action_for_event<Event>();
    }

    static constexpr const auto& conf = Conf;

private:
    using impl_type = simple_state_no_context<Conf>;

    static constexpr bool has_own_context = !std::is_same_v<context_type, null_t>;

    context_holder<context_type, context_sig> ctx_holder_;
    impl_type impl_;
};

} //namespace

#endif

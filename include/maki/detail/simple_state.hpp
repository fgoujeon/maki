//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "simple_state_fwd.hpp"
#include "context_holder.hpp"
#include "conf_traits.hpp"
#include "call_member.hpp"
#include "maybe_bool_util.hpp"
#include "event_action.hpp"
#include "tlu.hpp"
#include "../filters.hpp"
#include "../null.hpp"

namespace maki::detail
{

template<auto Id, class Context>
class simple_state_impl
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& conf = *Id;
    using option_set_type = std::decay_t<decltype(opts(conf))>;
    using context_type = typename option_set_type::context_type;

    static constexpr auto context_sig = opts(conf).context_sig;

    template<class... Args>
    simple_state_impl(Args&... args):
        ctx_holder_(args...)
    {
    }

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class ParentContext>
    auto& context_or(ParentContext& /*parent_ctx*/)
    {
        return context();
    }

    template<class ParentContext>
    const auto& context_or(ParentContext& /*parent_ctx*/) const
    {
        return context();
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_entry_action(mach, context(), event);
    }

    template<class Machine, class ParentContext, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event, MaybeBool&... processed)
    {
        impl_.call_internal_action(mach, context(), event, processed...);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_.call_exit_action(mach, context(), event);
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        return impl_type::template has_internal_action_for_event<Event>();
    }

private:
    using impl_type = simple_state_impl<identifier, void>;

    context_holder<context_type, context_sig> ctx_holder_;
    impl_type impl_;
};

template<auto Id>
class simple_state_impl<Id, void>
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& conf = *Id;
    using option_set_type = std::decay_t<decltype(opts(conf))>;

    static constexpr auto context_sig = opts(conf).context_sig;

    template<class... Args>
    simple_state_impl(Args&... /*args*/)
    {
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<entry_action_ptr_constant_list>)
        {
            /*
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_ptr_constant_list>
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
        static_assert(!tlu::empty_v<internal_action_ptr_constant_list>);

        call_matching_event_action<internal_action_ptr_constant_list>
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
        if constexpr(!tlu::empty_v<exit_action_ptr_constant_list>)
        {
            /*
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_ptr_constant_list>
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
            internal_action_ptr_constant_list,
            event_action_traits::for_event<Event>::template has_matching_event_filter
        >;
    }

private:
    static constexpr auto entry_actions = opts(conf).entry_actions;
    using entry_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = opts(conf).internal_actions;
    using internal_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = opts(conf).exit_actions;
    using exit_action_ptr_constant_list = tuple_to_element_ptr_constant_list_t<exit_actions>;
};

} //namespace

#endif

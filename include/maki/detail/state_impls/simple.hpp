//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "simple_fwd.hpp"
#include "../constant.hpp"
#include "../type_set.hpp"
#include "../event_action.hpp"
#include "../mix.hpp"
#include "../tlu/empty.hpp"
#include "../tlu/left_fold.hpp"
#include "../../context.hpp"
#include <type_traits>

namespace maki::detail::state_impls
{

template<class EventTypeSet, class EventAction>
using event_action_event_set_operation =
    maki::detail::type_set_union_t
    <
        EventTypeSet,
        typename EventAction::event_set_impl_type
    >
;

template<auto Id, class Context>
class simple
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using impl_type = simple<Id, void>;
    using context_type = Context;

    using event_type_set = typename impl_type::event_type_set;

    static constexpr auto context_sig = impl_of(mold).context_sig;

    template<class... Args>
    simple(Args&&... args):
        simple{constant<context_sig>, std::forward<Args>(args)...}
    {
    }

    simple(const simple&) = delete;
    simple(simple&&) = delete;
    simple& operator=(const simple&) = delete;
    simple& operator=(simple&&) = delete;
    ~simple() = default;

    context_type& context()
    {
        return ctx_;
    }

    const context_type& context() const
    {
        return ctx_;
    }

    template<class ParentContext>
    context_type& context_or(ParentContext& /*parent_ctx*/)
    {
        return ctx_;
    }

    template<class ParentContext>
    const context_type& context_or(ParentContext& /*parent_ctx*/) const
    {
        return ctx_;
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_type::call_entry_action(mach, ctx_, event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        return impl_type::template call_internal_action<Dry>(mach, ctx_, event);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action(Machine& mach, ParentContext& /*parent_ctx*/, const Event& event)
    {
        impl_type::call_exit_action(mach, ctx_, event);
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

private:
    template<class Machine, class... Args>
    simple
    (
        constant_t<machine_context_signature::a> /*tag*/,
        Machine& /*mach*/,
        Args&&... args
    ):
        ctx_{std::forward<Args>(args)...}
    {
    }

    template<class Machine, class... Args>
    simple
    (
        constant_t<machine_context_signature::am> /*tag*/,
        Machine& mach,
        Args&&... args
    ):
        ctx_{std::forward<Args>(args)..., mach}
    {
    }

    template<class Machine, class ParentContext>
    simple
    (
        constant_t<state_context_signature::c> /*tag*/,
        Machine& /*mach*/,
        ParentContext& parent_ctx
    ):
        ctx_{parent_ctx}
    {
    }

    template<class Machine, class ParentContext>
    simple
    (
        constant_t<state_context_signature::cm> /*tag*/,
        Machine& mach,
        ParentContext& parent_ctx
    ):
        ctx_{parent_ctx, mach}
    {
    }

    template<class Machine, class ParentContext>
    simple
    (
        constant_t<state_context_signature::m> /*tag*/,
        Machine& mach,
        ParentContext& /*parent_ctx*/
    ):
        ctx_{mach}
    {
    }

    template<class Machine, class ParentContext>
    simple
    (
        constant_t<state_context_signature::v> /*tag*/,
        Machine& /*mach*/,
        ParentContext& /*parent_ctx*/
    )
    {
    }

    Context ctx_;
    impl_type impl_;
};

template<auto Id>
class simple<Id, void>
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;

    using event_type_set =
        tlu::left_fold_t
        <
            typename option_set_type::internal_action_mix_type,
            event_action_event_set_operation,
            empty_type_set_t
        >
    ;

    template<class... Args>
    constexpr simple(Args&&... /*args*/)
    {
    }

    simple(const simple&) = delete;
    simple(simple&&) = delete;
    simple& operator=(const simple&) = delete;
    simple& operator=(simple&&) = delete;
    ~simple() = default;

    template<class ParentContext>
    static ParentContext& context_or(ParentContext& parent_ctx)
    {
        return parent_ctx;
    }

    template<class Machine, class Context, class Event>
    static void call_entry_action(Machine& mach, Context& ctx, const Event& event)
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

    template<bool Dry, class Machine, class Context, class Event>
    static bool call_internal_action(Machine& mach, Context& ctx, const Event& event)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_ptr_constant_list>);

        if constexpr(!Dry)
        {
            call_matching_event_action<internal_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }

        return true;
    }

    template<class Machine, class Context, class Event>
    static void call_exit_action(Machine& mach, Context& ctx, const Event& event)
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

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

private:
    static constexpr auto entry_actions = impl_of(mold).entry_actions;
    using entry_action_ptr_constant_list = mix_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = impl_of(mold).internal_actions;
    using internal_action_ptr_constant_list = mix_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = impl_of(mold).exit_actions;
    using exit_action_ptr_constant_list = mix_constant_list_t<exit_actions>;
};

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPL_HPP
#define MAKI_DETAIL_STATE_IMPL_HPP

#include "type_set.hpp"
#include "event_action.hpp"
#include "mix.hpp"
#include "tlu/empty.hpp"
#include "tlu/left_fold.hpp"
#include "context_holder.hpp"
#include "context_storage.hpp"
#include "../context.hpp"
#include <type_traits>

namespace maki::detail
{

template<class EventTypeSet, class EventAction>
using event_action_event_set_operation =
    maki::detail::type_set_union_t
    <
        EventTypeSet,
        typename EventAction::event_type_set
    >
;

template<auto Id, class ContextType, context_storage ParentCtxStorage>
class state_impl
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using context_type = ContextType;

    using event_type_set =
        tlu::left_fold_t
        <
            typename option_set_type::internal_action_mix_type,
            event_action_event_set_operation,
            empty_type_set_t
        >
    ;

    static constexpr auto context_sig = impl_of(mold).context_sig;

    template<class... Args>
    constexpr state_impl(Args&... args):
        ctx_holder_(args...)
    {
    }

    state_impl(const state_impl&) = delete;
    state_impl(state_impl&&) = delete;
    state_impl& operator=(const state_impl&) = delete;
    state_impl& operator=(state_impl&&) = delete;
    ~state_impl() = default;

    auto& context()
    {
        return ctx_holder_.get();
    }

    const auto& context() const
    {
        return ctx_holder_.get();
    }

    template<class ParentContext>
    auto& context_or(ParentContext& parent_ctx)
    {
        return ctx_holder_.get_or(parent_ctx);
    }

    template<class ParentContext>
    const auto& context_or(const ParentContext& parent_ctx) const
    {
        return ctx_holder_.get_or(parent_ctx);
    }

    template<class ParentContext>
    auto& deep_context_or(ParentContext& parent_ctx)
    {
        return ctx_holder_.get_deep_or(parent_ctx);
    }

    template<class ParentContext>
    const auto& deep_context_or(const ParentContext& parent_ctx) const
    {
        return ctx_holder_.get_deep_or(parent_ctx);
    }

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, Machine& mach)
    {
        emplace_contexts_with_parent_lifetime_2(*this, parent_ctx, mach);
    }

    template<class ParentContext, class Machine>
    void emplace_contexts_with_parent_lifetime(ParentContext& parent_ctx, Machine& mach) const
    {
        emplace_contexts_with_parent_lifetime_2(*this, parent_ctx, mach);
    }

    template<class Machine, class ParentContext, class Event>
    void enter(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        enter_2(*this, mach, parent_ctx, event);
    }

    template<class Machine, class ParentContext, class Event>
    void enter(Machine& mach, ParentContext& parent_ctx, const Event& event) const
    {
        enter_2(*this, mach, parent_ctx, event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
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
                ctx_holder_.get_deep_or(parent_ctx),
                event
            );
        }

        return true;
    }

    template<class Machine, class ParentContext, class Event>
    void exit(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        exit_2(*this, mach, parent_ctx, event);
    }

    template<class Machine, class ParentContext, class Event>
    void exit(Machine& mach, ParentContext& parent_ctx, const Event& event) const
    {
        exit_2(*this, mach, parent_ctx, event);
    }

    void reset_contexts_with_parent_lifetime()
    {
        reset_contexts_with_parent_lifetime_2(*this);
    }

    void reset_contexts_with_parent_lifetime() const
    {
        reset_contexts_with_parent_lifetime_2(*this);
    }

    static constexpr bool completed()
    {
        // Simple states are always completed.
        return true;
    }

private:
    template<class Self, class ParentContext, class Machine>
    static void emplace_contexts_with_parent_lifetime_2(Self& self, ParentContext& parent_ctx, Machine& mach)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            self.ctx_holder_.emplace(mach, parent_ctx);
        }
    }

    template<class Self, class Machine, class ParentContext, class Event>
    static void enter_2(Self& self, Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            self.ctx_holder_.emplace(mach, parent_ctx);
        }

        if constexpr(!tlu::empty_v<entry_action_ptr_constant_list>)
        {
            /*
            Execute entry action.
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<entry_action_ptr_constant_list>
            (
                mach,
                self.ctx_holder_.get_deep_or(parent_ctx),
                event
            );
        }
    }

    template<class Self, class Machine, class ParentContext, class Event>
    static void exit_2(Self& self, Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<exit_action_ptr_constant_list>)
        {
            /*
            Execute exit action.
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            call_matching_event_action<exit_action_ptr_constant_list>
            (
                mach,
                self.ctx_holder_.get_deep_or(parent_ctx),
                event
            );
        }

        if constexpr(ctx_lifetime == state_context_lifetime::state_activity)
        {
            self.ctx_holder_.reset();
        }
    }

    template<class Self>
    static void reset_contexts_with_parent_lifetime_2(Self& self)
    {
        if constexpr(ctx_lifetime == state_context_lifetime::parent)
        {
            self.ctx_holder_.reset();
        }
    }

    static constexpr auto entry_actions = impl_of(mold).entry_actions;
    using entry_action_ptr_constant_list = mix_constant_list_t<entry_actions>;

    static constexpr auto internal_actions = impl_of(mold).internal_actions;
    using internal_action_ptr_constant_list = mix_constant_list_t<internal_actions>;

    static constexpr auto exit_actions = impl_of(mold).exit_actions;
    using exit_action_ptr_constant_list = mix_constant_list_t<exit_actions>;

    static constexpr auto ctx_lifetime = impl_of(mold).context_lifetime;

    static constexpr auto ctx_storage =
        ctx_lifetime == state_context_lifetime::parent ?
        ParentCtxStorage :
        context_storage::optional
    ;

    context_holder<context_type, ctx_storage, context_sig> ctx_holder_;
};

template<auto Id>
using contextless_state_impl = state_impl<Id, void, context_storage::plain>;

} //namespace

#endif

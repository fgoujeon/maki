//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "../context_holder.hpp"
#include "../type_set.hpp"
#include "../event_action.hpp"
#include "../mix.hpp"
#include "../tlu/empty.hpp"
#include "../tlu/left_fold.hpp"
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

struct simple_from_context_args_tag_t{};
constexpr simple_from_context_args_tag_t simple_from_context_args_tag;

template<auto Id>
class simple
{
public:
    static constexpr auto identifier = Id;
    static constexpr const auto& mold = *Id;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;
    using context_type = typename option_set_type::context_type;

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
    constexpr simple(Args&&... args):
        ctx_holder_(std::forward<Args>(args)...)
    {
    }

    simple(const simple&) = delete;
    simple(simple&&) = delete;
    simple& operator=(const simple&) = delete;
    simple& operator=(simple&&) = delete;
    ~simple() = default;

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
        return get_context_or(ctx_holder_, parent_ctx);
    }

    template<class ParentContext>
    const auto& context_or(ParentContext& parent_ctx) const
    {
        return get_context_or(ctx_holder_, parent_ctx);
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        call_entry_action_2(mach, context_or(parent_ctx), event);
    }

    template<class Machine, class ParentContext, class Event>
    void call_entry_action(Machine& mach, ParentContext& parent_ctx, const Event& event) const
    {
        call_entry_action_2(mach, context_or(parent_ctx), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        return call_internal_action_2<Dry>(mach, context_or(parent_ctx), event);
    }

    template<bool Dry, class Machine, class ParentContext, class Event>
    bool call_internal_action(Machine& mach, ParentContext& parent_ctx, const Event& event) const
    {
        return call_internal_action_2<Dry>(mach, context_or(parent_ctx), event);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action(Machine& mach, ParentContext& parent_ctx, const Event& event)
    {
        call_exit_action_2(mach, context_or(parent_ctx), event);
    }

    template<class Machine, class ParentContext, class Event>
    void call_exit_action(Machine& mach, ParentContext& parent_ctx, const Event& event) const
    {
        call_exit_action_2(mach, context_or(parent_ctx), event);
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

    template<class Machine, class Context, class Event>
    static void call_entry_action_2
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
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
    static bool call_internal_action_2
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
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
    static void call_exit_action_2
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
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

    context_holder<context_type, context_sig> ctx_holder_;
};

} //namespace

#endif

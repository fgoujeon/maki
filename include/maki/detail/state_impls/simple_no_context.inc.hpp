//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail::state_impls
{

template<class EventTypeSet, class EventAction>
using MAKI_AOS_NAME(event_action_event_set_operation) =
    maki::detail::type_set_union_t
    <
        EventTypeSet,
        typename EventAction::event_type_set
    >
;

template<class MachineConfHolder, class StateMoldPath>
class MAKI_AOS_NAME(simple_no_context)
{
public:
    using machine_conf_holder_type = MachineConfHolder;
    using state_mold_path = StateMoldPath;

    static constexpr const auto& mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;
    using option_set_type = std::decay_t<decltype(impl_of(mold))>;

    using event_type_set =
        tlu::left_fold_t
        <
            typename option_set_type::internal_action_mix_type,
            MAKI_AOS_NAME(event_action_event_set_operation),
            empty_type_set_t
        >
    ;

    using deferrable_event_type_set =
        typename option_set_type::deferred_event_type_set
    ;

    template<class... Args>
    constexpr MAKI_AOS_NAME(simple_no_context)(Args&... /*args*/)
    {
    }

    MAKI_AOS_NAME(simple_no_context)(const MAKI_AOS_NAME(simple_no_context)&) = default;
    MAKI_AOS_NAME(simple_no_context)(MAKI_AOS_NAME(simple_no_context)&&) = default;
    MAKI_AOS_NAME(simple_no_context)& operator=(const MAKI_AOS_NAME(simple_no_context)&) = default;
    MAKI_AOS_NAME(simple_no_context)& operator=(MAKI_AOS_NAME(simple_no_context)&&) = default;
    ~MAKI_AOS_NAME(simple_no_context)() = default;

    template<class Event>
    [[nodiscard]] static constexpr bool defers_event()
    {
        if constexpr(type_set_contains_v<deferrable_event_type_set, Event>)
        {
            return type_set_contains_v
            <
                deferrable_event_type_set,
                Event
            >;
        }
        else
        {
            return false;
        }
    }

    template<class ParentContext, class Machine>
    static constexpr void emplace_contexts_with_parent_lifetime(ParentContext& /*parent_ctx*/, Machine& /*mach*/)
    {
        // No context to emplace
    }

    template<class R, class Machine, class Context, class Event>
    static R enter(Machine& mach, Context& ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<entry_action_ptr_constant_list>)
        {
            /*
            Execute entry action.
            If at least one entry action is defined, state is required to define
            entry actions for all possible event types.
            */
            MAKI_AOS_CALL call_matching_event_action<entry_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }
    }

    template<bool Dry, class Machine, class Context, class Event>
    static MAKI_AOS_BOOL call_internal_action(Machine& mach, Context& ctx, const Event& event)
    {
        /*
        Caller is supposed to check an interal action exists for the given event
        type before calling this function.
        */
        static_assert(!tlu::empty_v<internal_action_ptr_constant_list>);

        if constexpr(!Dry)
        {
            MAKI_AOS_CALL call_matching_event_action<internal_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }

        MAKI_AOS_RETURN true;
    }

    template<class R, class Machine, class Context, class Event>
    static R exit(Machine& mach, Context& ctx, const Event& event)
    {
        if constexpr(!tlu::empty_v<exit_action_ptr_constant_list>)
        {
            /*
            Execute exit action.
            If at least one exit action is defined, state is required to define
            entry actions for all possible event types.
            */
            MAKI_AOS_CALL call_matching_event_action<exit_action_ptr_constant_list>
            (
                mach,
                ctx,
                event
            );
        }
    }

    static constexpr void reset_contexts_with_parent_lifetime()
    {
        // No context to reset
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

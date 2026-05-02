//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_IMPL_HPP
#define MAKI_DETAIL_REGION_IMPL_HPP

#include "state_mold_indexes.hpp"
#include "path_to_transition_table.hpp"
#include "compiler.hpp"
#include "type_set.hpp"
#include "state_id_to_state.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "context_storage.hpp"
#include "equals.hpp"
#include "tuple.hpp"
#include "mix.hpp"
#include "index_sequence.hpp"
#include "friendly_impl.hpp"
#include "../states.hpp"
#include "../action.hpp"
#include "../guard.hpp"
#include "../path.hpp"
#include "../null.hpp"
#include "../state_mold.hpp"
#include "../state.hpp"
#include "../transition_table.hpp"
#include <type_traits>

namespace maki
{
    template<class Impl>
    class region;
}

namespace maki::detail
{

namespace region_detail
{
    inline constexpr auto null_action_index = -1;
    inline constexpr auto null_guard_index = -1;

    template<const auto& TransitionTable, int StateMoldIndex>
    constexpr auto state_mold_index_to_state_mold()
    {
        if constexpr(StateMoldIndex == state_mold_indexes::undefined)
        {
            return &maki::undefined;
        }
        else if constexpr(StateMoldIndex == state_mold_indexes::null)
        {
            return &state_molds::null;
        }
        else if constexpr(StateMoldIndex == state_mold_indexes::fin)
        {
            return &state_molds::fin;
        }
        else
        {
            return tuple_get<StateMoldIndex>(impl_of(TransitionTable)).target_state_mold;
        }
    }

    template<const auto& TransitionTable, auto StateMold, int TransitionIndex>
    constexpr int state_mold_to_state_mold_index_2()
    {
        if constexpr(ptr_equals(StateMold, tuple_get<TransitionIndex>(impl_of(TransitionTable)).target_state_mold))
        {
            return TransitionIndex;
        }
        else
        {
            return state_mold_to_state_mold_index_2<TransitionTable, StateMold, TransitionIndex + 1>();
        }
    }

    template<const auto& TransitionTable, auto StateMold>
    constexpr int state_mold_to_state_mold_index()
    {
        if constexpr(ptr_equals(StateMold, &maki::undefined))
        {
            return state_mold_indexes::undefined;
        }
        else if constexpr(ptr_equals(StateMold, &state_molds::null))
        {
            return state_mold_indexes::null;
        }
        else if constexpr(ptr_equals(StateMold, &state_molds::fin))
        {
            return state_mold_indexes::fin;
        }
        else
        {
            return state_mold_to_state_mold_index_2<TransitionTable, StateMold, 0>();
        }
    }

    namespace filter_state_mold_index_sequence_by_state_set_detail
    {
        template<const auto& TransitionTable, auto StateSetPtr>
        struct for_state_set
        {
            template<int StateMoldIndex>
            struct matches
            {
                static constexpr auto state_id = state_mold_index_to_state_mold<TransitionTable, StateMoldIndex>();
                static constexpr bool value = contains(impl_of(*StateSetPtr), state_id);
            };
        };
    }

    template<const auto& TransitionTable, class StateMoldIndexSequence, auto StateSetPtr>
    using filter_state_mold_index_sequence_by_state_set_t = index_sequence_filter_t
    <
        StateMoldIndexSequence,
        filter_state_mold_index_sequence_by_state_set_detail::for_state_set<TransitionTable, StateSetPtr>::template matches
    >;
}

template<const auto& MachineConf, class TransitionTablePath, const auto& Path, context_storage ParentCtxStorage>
class region_impl
{
public:
    static constexpr auto trans_table = path_to_transition_table_v<MachineConf, TransitionTablePath>;

    using transition_table_type = std::decay_t<decltype(trans_table)>;

    using transition_table_digest_type =
        transition_table_digest<trans_table>
    ;

    using state_mold_index_sequence_0 =
        typename transition_table_digest_type::unique_target_state_mold_index_sequence
    ;

    using state_mold_index_sequence =
        index_sequence_push_back_t
        <
            state_mold_index_sequence_0,
            state_mold_indexes::undefined
        >
    ;

    template<int... StateMoldIndexes>
    using state_mold_index_sequence_to_state_mix_t = mix
    <
        state_traits::state_id_to_state_t
        <
            MachineConf,
            index_sequence_push_back_t<TransitionTablePath, StateMoldIndexes>,
            Path,
            ParentCtxStorage
        >...
    >;

    using state_mix_type = index_sequence_apply_t
    <
        state_mold_index_sequence_0,
        state_mold_index_sequence_to_state_mix_t
    >;

    using states_event_type_set = state_type_list_event_type_set_t<state_mix_type>;

    using event_type_set = type_set_union_t
    <
        transition_table_event_type_set_t<transition_table_type>,
        states_event_type_set
    >;

    using deferrable_event_type_set = state_type_list_deferrable_event_type_set_t<state_mix_type>;

    template<class Machine, class Context>
    region_impl(const region<region_impl>* pitf, Machine& mach, Context& ctx):
        pitf_(pitf),
        states_(mix_uniform_construct, mach, ctx)
    {
    }

    region_impl(const region_impl&) = delete;
    region_impl(region_impl&&) = delete;
    region_impl& operator=(const region_impl&) = delete;
    region_impl& operator=(region_impl&&) = delete;
    ~region_impl() = default;

    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        if constexpr(is_state_set_v<std::decay_t<decltype(StateMold)>>)
        {
            return is_active_state_id_in_set<&StateMold>();
        }
        else
        {
            return is_active_state_id<&StateMold>();
        }
    }

    [[nodiscard]] bool completed() const
    {
        return active_state_mold_index_ == state_mold_indexes::fin;
    }

    template<class Event>
    [[nodiscard]] bool defers_event() const
    {
        if constexpr(type_set_contains_v<deferrable_event_type_set, Event>)
        {
            auto defers = false;
            with_active_state_id
            <
                state_mold_index_sequence,
                state_defers_event<Event>
            >(*this, defers);
            return defers;
        }
        else
        {
            return false;
        }
    }

    template<class Context, class Machine>
    void emplace_contexts_with_parent_lifetime(Context& ctx, Machine& mach)
    {
        tlu::for_each
        <
            state_mix_type,
            state_emplace_contexts_with_parent_lifetime
        >(*this, ctx, mach);
    }

    // Enter the initial state
    template<class Machine, class Context, class Event>
    void enter(Machine& mach, Context& ctx, const Event& event)
    {
        execute_transition
        <
            state_mold_indexes::null,
            state_mold_indexes::ini,
            0
        >
        (
            mach,
            ctx,
            event
        );
    }

    // Exit the active state
    template<int TargetStateMoldIndex, class Machine, class Context, class Event>
    void exit(Machine& mach, Context& ctx, const Event& event)
    {
        if(!completed())
        {
            with_active_state_id<state_mold_index_sequence, exit_2<TargetStateMoldIndex>>
            (
                *this,
                mach,
                ctx,
                event
            );
        }
    }

    void reset_contexts_with_parent_lifetime()
    {
        tlu::for_each
        <
            state_mix_type,
            state_reset_contexts_with_parent_lifetime
        >(*this);
    }

    template<bool Dry, class Machine, class Context, class Event>
    bool process_event(Machine& mach, Context& ctx, const Event& event)
    {
        return process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<bool Dry, class Machine, class Context, class Event>
    bool process_event(Machine& mach, Context& ctx, const Event& event) const
    {
        return process_event_2<Dry>(*this, mach, ctx, event);
    }

    template<const auto& StateMold>
    const auto& state() const
    {
        return state_id_to_obj<&StateMold>();
    }

    static const auto& path()
    {
        static const auto value = maki::path{Path};
        return value;
    }

private:
    struct state_emplace_contexts_with_parent_lifetime
    {
        template<class State, class Self, class Context, class Machine>
        static void call
        (
            Self& self,
            Context& ctx,
            Machine& mach
        )
        {
            auto& stt = self.template state_type_to_obj<State>();
            impl_of(stt).emplace_contexts_with_parent_lifetime(ctx, mach);
        }
    };

    struct state_reset_contexts_with_parent_lifetime
    {
        template<class State, class Self>
        static void call(Self& self)
        {
            auto& stt = self.template state_type_to_obj<State>();
            impl_of(stt).reset_contexts_with_parent_lifetime();
        }
    };

    template<class Event>
    struct state_defers_event
    {
        template<int StateMoldIndex>
        static void call(const region_impl& self, bool& defers)
        {
            const auto& stt = self.state_mold_index_to_state<StateMoldIndex>();
            defers = impl_of(stt).template defers_event<Event>();
        }
    };

    template<bool Dry, class Self, class Machine, class Context, class Event>
    static bool process_event_2
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        //List the transitions whose event set contains `Event`
        using candidate_transition_index_constant_list = transition_table_filters::by_event_t
        <
#if !MAKI_DETAIL_COMPILER_GCC
            transition_table_type,
#else
            trans_table,
#endif
            Event
        >;

        constexpr auto must_try_executing_transitions = !tlu::empty_v<candidate_transition_index_constant_list>;

        constexpr auto must_try_process_event_in_states = type_set_contains_v
        <
            states_event_type_set,
            Event
        >;

        if constexpr(must_try_executing_transitions && must_try_process_event_in_states)
        {
            /*
            There is a possibility of conflicting transition in this case.
            Note that nested transitions take precedence over higher-order
            transitions.
            */
            return
                call_active_state_internal_action<Dry>(self, mach, ctx, event) ||
                try_executing_transitions<candidate_transition_index_constant_list, Dry>(self, mach, ctx, event)
            ;
        }
        else if constexpr(!must_try_executing_transitions && must_try_process_event_in_states)
        {
            return call_active_state_internal_action<Dry>(self, mach, ctx, event);
        }
        else if constexpr(must_try_executing_transitions && !must_try_process_event_in_states)
        {
            return try_executing_transitions<candidate_transition_index_constant_list, Dry>(self, mach, ctx, event);
        }
        else
        {
            return false;
        }
    }

    template<int TargetStateMoldIndex>
    struct exit_2
    {
        template<int ActiveStateMoldIndex, class Machine, class Context, class Event>
        static void call(region_impl& self, Machine& mach, Context& ctx, const Event& event)
        {
            self.execute_transition
            <
                ActiveStateMoldIndex,
                TargetStateMoldIndex,
                region_detail::null_action_index
            >(mach, ctx, event);
        }
    };

    /*
    Try executing one of the transitions at indices
    `TransitionIndexConstantList`.
    */
    template<class TransitionIndexConstantList, bool Dry = false, class Self, class Machine, class Context, class Event>
    static bool try_executing_transitions(Self& self, Machine& mach, Context& ctx, const Event& event)
    {
        return tlu::for_each_or
        <
            TransitionIndexConstantList,
            try_executing_transition<Dry>
        >(self, mach, ctx, event);
    }

    // Try executing the transition at index `TransitionIndexConstant`.
    template<bool Dry>
    struct try_executing_transition
    {
        template<class TransitionIndexConstant, class Self, class Machine, class Context, class Event, class... ExtraArgs>
        static bool call(Self& self, Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
        {
            static constexpr const auto& trans = tuple_get<TransitionIndexConstant::value>(impl_of(trans_table));
            static constexpr auto source_state_mold = trans.source_state_mold;

            static constexpr auto target_state_mold_index =
                region_detail::state_mold_to_state_mold_index
                <
                    trans_table,
                    trans.target_state_mold
                >()
            ;

            if constexpr(is_state_set_v<std::decay_t<decltype(source_state_mold)>>)
            {
                //List of state molds that belong to the source state set
                using matching_state_mold_index_sequence = region_detail::filter_state_mold_index_sequence_by_state_set_t
                <
                    trans_table,
                    state_mold_index_sequence,
                    &source_state_mold
                >;

                static_assert(index_sequence_size_v<matching_state_mold_index_sequence> != 0);

                return index_sequence_for_each_or
                <
                    matching_state_mold_index_sequence,
                    try_executing_transition_2
                    <
                        Dry,
                        target_state_mold_index,
                        TransitionIndexConstant::value,
                        TransitionIndexConstant::value
                    >
                >(self, mach, ctx, event, extra_args...);
            }
            else
            {
                static constexpr auto source_state_mold_index =
                    region_detail::state_mold_to_state_mold_index
                    <
                        trans_table,
                        trans.source_state_mold
                    >()
                ;

                return try_executing_transition_2
                <
                    Dry,
                    target_state_mold_index,
                    TransitionIndexConstant::value,
                    TransitionIndexConstant::value
                >::template call<source_state_mold_index>
                (
                    self,
                    mach,
                    ctx,
                    event
                );
            }
        }
    };

    template<bool Dry, int TargetStateMoldIndex, int ActionIndex, int GuardIndex>
    struct try_executing_transition_2
    {
        template
        <
            int SourceStateMoldIndex,
            class Self,
            class Machine,
            class Context,
            class Event
        >
        static bool call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event
        )
        {
            if constexpr(!is_null_v<Event>) // Already filtered out
            {
                //Make sure the transition source state is the active state
                if(self.active_state_mold_index_ != SourceStateMoldIndex)
                {
                    return false;
                }
            }

            //Check guard
            if constexpr(GuardIndex != region_detail::null_guard_index)
            {
                const auto& guard = tuple_get<GuardIndex>(impl_of(trans_table)).grd;
                if(!detail::call_guard(guard, ctx, mach, event))
                {
                    return false;
                }
            }

            if constexpr(!Dry)
            {
                self.template execute_transition
                <
                    SourceStateMoldIndex,
                    TargetStateMoldIndex,
                    ActionIndex
                >(mach, ctx, event);
            }

            return true;
        }
    };

    template
    <
        int SourceStateMoldIndex,
        int TargetStateMoldIndex,
        int ActionIndex,
        class Machine,
        class Context,
        class Event
    >
    void execute_transition
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        using machine_option_set_type = typename Machine::option_set_type;

        constexpr auto target_state_mold = region_detail::state_mold_index_to_state_mold<trans_table, TargetStateMoldIndex>();

        constexpr auto is_external_transition = !is_null_v
        <
            std::decay_t<decltype(target_state_mold)>
        >;

        auto& source_state = state_mold_index_to_state<SourceStateMoldIndex>();

        /*
        For external transitions, invoke the pre-transition hook, if any.
        */
        if constexpr
        (
            is_external_transition &&
            !is_null_v<typename machine_option_set_type::pre_external_transition_hook_type>
        )
        {
            impl_of(Machine::conf).pre_external_transition_hook
            (
                ctx,
                *pitf_,
                source_state,
                state_mold_index_to_state<TargetStateMoldIndex>(),
                event
            );
        }

        /*
        For external transitions, change the active state to `undefined` (useful
        in case of exception).
        */
        if constexpr
        (
            is_external_transition &&
            TargetStateMoldIndex != state_mold_indexes::null
        )
        {
            active_state_mold_index_ = state_mold_indexes::undefined;
        }

        /*
        For external transitions, invoke the exit action of the source state, if
        any.
        */
        if constexpr(is_external_transition)
        {
            impl_of(source_state).exit
            (
                mach,
                ctx,
                event
            );
        }

        /*
        Invoke the transition action, if any.
        */
        if constexpr(ActionIndex != region_detail::null_action_index)
        {
            detail::call_action
            (
                tuple_get<ActionIndex>(impl_of(trans_table)).act,
                ctx,
                mach,
                event
            );
        }

        /*
        For external transitions, invoke the entry action of the target state,
        if any.
        */
        if constexpr(is_external_transition)
        {
            auto& target_state = state_mold_index_to_state<TargetStateMoldIndex>();

            impl_of(target_state).enter
            (
                mach,
                ctx,
                event
            );
        }

        /*
        For external transitions, change the active state to the target state.
        */
        if constexpr
        (
            is_external_transition &&
            TargetStateMoldIndex != state_mold_indexes::null
        )
        {
            active_state_mold_index_ = TargetStateMoldIndex;
        }

        /*
        For external transitions, invoke the post-transition hook, if any.
        */
        if constexpr
        (
            is_external_transition &&
            !is_null_v<typename machine_option_set_type::post_external_transition_hook_type>
        )
        {
            impl_of(Machine::conf).post_external_transition_hook
            (
                ctx,
                *pitf_,
                source_state,
                state_mold_index_to_state<TargetStateMoldIndex>(),
                event
            );
        }

        /*
        For external transitions, execute the completion transitions, if any.
        */
        if constexpr
        (
            is_external_transition &&
            transition_table_digest_type::has_completion_transitions &&
            !ptr_equals(target_state_mold, &state_molds::null)
        )
        {
            try_executing_completion_transitions
            (
                state_mold_index_to_state<TargetStateMoldIndex>(),
                mach,
                ctx
            );
        }
    }

    // Find the active state and call its internal action for `event`.
    template<bool Dry, class Self, class Machine, class Context, class Event>
    static bool call_active_state_internal_action
    (
        Self& self,
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        auto processed = false;
        tlu::for_each_or
        <
            state_mix_type,
            call_active_state_internal_action_2<Dry>
        >(self, mach, ctx, event, processed);
        return processed;
    }

    template<bool Dry>
    struct call_active_state_internal_action_2
    {
        template<class State, class Self, class Machine, class Context, class Event>
        static bool call
        (
            Self& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            bool& processed
        )
        {
            constexpr auto can_state_process_event =
                type_set_contains_v
                <
                    typename impl_of_t<State>::event_type_set,
                    Event
                >
            ;

            if constexpr(can_state_process_event)
            {
                if(!self.template is_active_state_type<State>())
                {
                    return false;
                }

                auto& active_state = self.template state_type_to_obj<State>();

                processed = impl_of(active_state).template call_internal_action<Dry>
                (
                    mach,
                    ctx,
                    event
                );

                if constexpr
                (
                    transition_table_digest_type::has_completion_transitions &&
                    !Dry
                )
                {
                    self.try_executing_completion_transitions
                    (
                        active_state,
                        mach,
                        ctx
                    );
                }

                return true;
            }
            else
            {
                return false;
            }
        }
    };

    template<class ActiveState, class Machine, class Context>
    void try_executing_completion_transitions
    (
        ActiveState& active_state,
        Machine& mach,
        Context& ctx
    )
    {
        constexpr auto active_state_id = impl_of_t<ActiveState>::identifier;

        using candidate_transition_index_constant_list = transition_table_filters::by_source_state_and_null_event_t
        <
            trans_table,
            active_state_id
        >;

        if constexpr(!tlu::empty_v<candidate_transition_index_constant_list>)
        {
            if(impl_of(active_state).completed())
            {
                try_executing_transitions<candidate_transition_index_constant_list>(*this, mach, ctx, null);
            }
        }
    }

    template<class State>
    [[nodiscard]] bool is_active_state_type() const
    {
        return is_active_state_id<impl_of_t<State>::identifier>();
    }

    template<auto StateId>
    [[nodiscard]] bool is_active_state_id() const
    {
        return active_state_mold_index_ == region_detail::state_mold_to_state_mold_index<trans_table, StateId>();
    }

    template<auto StateSetPtr>
    [[nodiscard]] bool is_active_state_id_in_set() const
    {
        auto matches = false;
        with_active_state_id
        <
            index_sequence_push_back_t
            <
                state_mold_index_sequence,
                state_mold_indexes::fin
            >,
            is_active_state_id_in_set_2<StateSetPtr>
        >(matches);
        return matches;
    }

    template<auto StateSetPtr>
    struct is_active_state_id_in_set_2
    {
        template<int ActiveStateMoldIndex>
        static void call([[maybe_unused]] bool& matches)
        {
            constexpr auto active_state_mold = region_detail::state_mold_index_to_state_mold<trans_table, ActiveStateMoldIndex>();
            if constexpr(contains(impl_of(*StateSetPtr), active_state_mold))
            {
                matches = true;
            }
        }
    };

    template<class StateMoldIndexSequence, class F, class... Args>
    void with_active_state_id(Args&&... args) const
    {
        index_sequence_for_each_or
        <
            StateMoldIndexSequence,
            with_active_state_id_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_id_2
    {
        template<int StateMoldIndex, class... Args>
        static bool call(const region_impl& self, Args&&... args)
        {
            if(self.active_state_mold_index_ == StateMoldIndex)
            {
                F::template call<StateMoldIndex>(std::forward<Args>(args)...);
                return true;
            }
            return false;
        }
    };

    template<class State>
    auto& state_type_to_obj()
    {
        return static_state_type_to_obj<State>(*this);
    }

    template<class State>
    const auto& state_type_to_obj() const
    {
        return static_state_type_to_obj<State>(*this);
    }

    template<auto StateId>
    auto& state_id_to_obj()
    {
        return static_state_id_to_obj<StateId>(*this);
    }

    template<auto StateId>
    const auto& state_id_to_obj() const
    {
        return static_state_id_to_obj<StateId>(*this);
    }

    //Note: We use static to factorize const and non-const Region
    template<class State, class Region>
    static auto& static_state_type_to_obj(Region& self)
    {
        return static_state_id_to_obj<impl_of_t<State>::identifier>(self);
    }

    //Note: We use static to factorize const and non-const Region
    template<auto StateId, class Region>
    static auto& static_state_id_to_obj(Region& self)
    {
        if constexpr(ptr_equals(StateId, &state_molds::null))
        {
            return states::null;
        }
        else if constexpr(ptr_equals(StateId, &maki::undefined))
        {
            return states::undefined;
        }
        else if constexpr(ptr_equals(StateId, &state_molds::fin))
        {
            return states::fin;
        }
        else
        {
            constexpr int state_mold_index = region_detail::state_mold_to_state_mold_index<trans_table, StateId>();
            using state_t =
                state_traits::state_id_to_state_t
                <
                    MachineConf,
                    index_sequence_push_back_t<TransitionTablePath, state_mold_index>,
                    Path,
                    ParentCtxStorage
                >
            ;
            return get<state_t>(self.states_);
        }
    }

    template<int StateMoldIndex>
    auto& state_mold_index_to_state()
    {
        return static_state_mold_index_to_state<StateMoldIndex>(*this);
    }

    template<int StateMoldIndex>
    const auto& state_mold_index_to_state() const
    {
        return static_state_mold_index_to_state<StateMoldIndex>(*this);
    }

    //Note: We use static to factorize const and non-const Region
    template<int StateMoldIndex, class Region>
    static auto& static_state_mold_index_to_state(Region& self)
    {
        constexpr auto state_mold = region_detail::state_mold_index_to_state_mold<trans_table, StateMoldIndex>();
        return static_state_id_to_obj<state_mold>(self);
    }

    const region<region_impl>* pitf_;
    state_mix_type states_;
    int active_state_mold_index_ = state_mold_indexes::fin;
};

} //namespace

#endif

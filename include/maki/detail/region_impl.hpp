//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_IMPL_HPP
#define MAKI_DETAIL_REGION_IMPL_HPP

#include "compiler.hpp"
#include "type_set.hpp"
#include "state_id_to_state.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "state_type_list_filters.hpp"
#include "equals.hpp"
#include "tuple.hpp"
#include "mix.hpp"
#include "constant.hpp"
#include "friendly_impl.hpp"
#include "tlu/apply.hpp"
#include "tlu/empty.hpp"
#include "tlu/find.hpp"
#include "tlu/front.hpp"
#include "tlu/push_back.hpp"
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
    inline constexpr auto final_state_index = -1;

    template<class StateIdConstantList, auto StateId>
    struct state_id_to_index
    {
        static constexpr auto value = tlu::find_v<StateIdConstantList, constant_t<StateId>>;
    };

    template<class StateIdConstantList>
    struct state_id_to_index<StateIdConstantList, &state_molds::fin>
    {
        static constexpr auto value = final_state_index;
    };

    template<class StateIdConstantList, auto StateId>
    inline constexpr auto state_id_to_index_v = state_id_to_index<StateIdConstantList, StateId>::value;
}

template<const auto& TransitionTable, const auto& Path>
class region_impl
{
public:
    using transition_table_type = std::decay_t<decltype(TransitionTable)>;

    using transition_table_digest_type =
        transition_table_digest<TransitionTable>
    ;

    using state_id_constant_list_0 = typename transition_table_digest_type::state_id_constant_list;
    using state_id_constant_list = tlu::push_back_t<state_id_constant_list_0, constant_t<&maki::undefined>>;

    template<class... StateIdConstants>
    using state_id_constant_pack_to_state_mix_t = mix
    <
        state_traits::state_id_to_state_t<StateIdConstants::value, Path>...
    >;

    using state_mix_type = tlu::apply_t
    <
        state_id_constant_list,
        state_id_constant_pack_to_state_mix_t
    >;

    using states_event_type_set = state_type_list_event_type_set_t<state_mix_type>;

    using event_type_set = type_set_union_t
    <
        transition_table_event_type_set_t<transition_table_type>,
        states_event_type_set
    >;

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
        return is_active_state_id<&state_molds::fin>();
    }

    // Enter the initial state
    template<class Machine, class Context, class Event>
    void enter(Machine& mach, Context& ctx, const Event& event)
    {
        static constexpr auto initial_state_id = tlu::front_t<state_id_constant_list>::value;
        static constexpr auto action = tuple_get<0>(impl_of(TransitionTable)).act;

        execute_transition
        <
            &state_molds::null,
            initial_state_id,
            &action
        >
        (
            mach,
            ctx,
            event
        );
    }

    // Exit the active state
    template<auto TargetStateId, class Machine, class Context, class Event>
    void exit(Machine& mach, Context& ctx, const Event& event)
    {
        if(!completed())
        {
            with_active_state_id<state_id_constant_list, exit_2<TargetStateId>>
            (
                *this,
                mach,
                ctx,
                event
            );
        }
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
            TransitionTable,
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

    template<auto TargetStateId>
    struct exit_2
    {
        template<class ActiveStateIdConstant, class Machine, class Context, class Event>
        static void call(region_impl& self, Machine& mach, Context& ctx, const Event& event)
        {
            self.execute_transition
            <
                ActiveStateIdConstant::value,
                TargetStateId,
                &null_action
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
            static constexpr const auto& trans = tuple_get<TransitionIndexConstant::value>(impl_of(TransitionTable));
            static constexpr auto source_state_mold = trans.source_state_mold;
            static constexpr auto action = trans.act;
            static constexpr auto guard = trans.grd;

            if constexpr(is_state_set_v<std::decay_t<decltype(source_state_mold)>>)
            {
                //List of state molds that belong to the source state set
                using matching_state_mold_constant_list = state_type_list_filters::by_state_set_t
                <
                    state_id_constant_list,
                    &source_state_mold
                >;

                static_assert(!tlu::empty_v<matching_state_mold_constant_list>);

                return tlu::for_each_or
                <
                    matching_state_mold_constant_list,
                    try_executing_transition_2
                    <
                        Dry,
                        trans.target_state_mold,
                        action,
                        guard
                    >
                >(self, mach, ctx, event, extra_args...);
            }
            else
            {
                return try_executing_transition_2
                <
                    Dry,
                    trans.target_state_mold,
                    action,
                    guard
                >::template call<constant_t<trans.source_state_mold>>
                (
                    self,
                    mach,
                    ctx,
                    event
                );
            }
        }
    };

    template<bool Dry, auto TargetStateId, const auto& Action, const auto& Guard>
    struct try_executing_transition_2
    {
        template
        <
            class SourceStateIdConstant,
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
                if(!self.template is_active_state_id<SourceStateIdConstant::value>())
                {
                    return false;
                }
            }

            //Check guard
            if constexpr(!std::is_same_v<decltype(Guard), const null_t&>)
            {
                if(!detail::call_guard(Guard, ctx, mach, event))
                {
                    return false;
                }
            }

            if constexpr(!Dry)
            {
                self.template execute_transition
                <
                    SourceStateIdConstant::value,
                    TargetStateId,
                    &Action
                >(mach, ctx, event);
            }

            return true;
        }
    };

    template
    <
        auto SourceStateId,
        auto TargetStateId,
        auto ActionPtr,
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

        constexpr auto is_external_transition = !is_null_v
        <
            std::decay_t<decltype(TargetStateId)>
        >;

        auto& source_state = state_id_to_obj<SourceStateId>();

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
                state_id_to_obj<TargetStateId>(),
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
            !ptr_equals(TargetStateId, &state_molds::null)
        )
        {
            active_state_index_ = region_detail::state_id_to_index_v
            <
                state_id_constant_list,
                &maki::undefined
            >;
        }

        /*
        For external transitions, invoke the exit action of the source state, if
        any.
        */
        if constexpr(is_external_transition)
        {
            impl_of(source_state).call_exit_action
            (
                mach,
                ctx,
                event
            );
        }

        /*
        Invoke the transition action, if any.
        */
        detail::call_action
        (
            *ActionPtr,
            ctx,
            mach,
            event
        );

        /*
        For external transitions, invoke the entry action of the target state,
        if any.
        */
        if constexpr(is_external_transition)
        {
            auto& target_state = state_id_to_obj<TargetStateId>();

            impl_of(target_state).call_entry_action
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
            !ptr_equals(TargetStateId, &state_molds::null)
        )
        {
            active_state_index_ = region_detail::state_id_to_index_v
            <
                state_id_constant_list,
                TargetStateId
            >;
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
                state_id_to_obj<TargetStateId>(),
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
            !ptr_equals(TargetStateId, &state_molds::null)
        )
        {
            try_executing_completion_transitions
            (
                state_id_to_obj<TargetStateId>(),
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
            TransitionTable,
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
        constexpr auto given_state_index = region_detail::state_id_to_index_v
        <
            state_id_constant_list,
            StateId
        >;
        return given_state_index == active_state_index_;
    }

    template<auto StateSetPtr>
    [[nodiscard]] bool is_active_state_id_in_set() const
    {
        auto matches = false;
        with_active_state_id
        <
            tlu::push_back_t<state_id_constant_list, constant_t<&state_molds::fin>>,
            is_active_state_id_in_set_2<StateSetPtr>
        >(matches);
        return matches;
    }

    template<auto StateSetPtr>
    struct is_active_state_id_in_set_2
    {
        template<class ActiveStateIdConstant>
        static void call([[maybe_unused]] bool& matches)
        {
            if constexpr(contains(impl_of(*StateSetPtr), ActiveStateIdConstant::value))
            {
                matches = true;
            }
        }
    };

    template<class StateIdConstantList, class F, class... Args>
    void with_active_state_id(Args&&... args) const
    {
        tlu::for_each_or
        <
            StateIdConstantList,
            with_active_state_id_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_id_2
    {
        template<class StateIdConstant, class... Args>
        static bool call(const region_impl& self, Args&&... args)
        {
            if(self.is_active_state_id<StateIdConstant::value>())
            {
                F::template call<StateIdConstant>(std::forward<Args>(args)...);
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
            using state_t =
                state_traits::state_id_to_state_t<StateId, Path>
            ;
            return get<state_t>(self.states_);
        }
    }

    const region<region_impl>* pitf_;
    state_mix_type states_;
    int active_state_index_ = region_detail::final_state_index;
};

} //namespace

#endif

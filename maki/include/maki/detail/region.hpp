//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_HPP
#define MAKI_DETAIL_REGION_HPP

#include "region_path_of.hpp"
#include "state_traits.hpp"
#include "call_member.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "state_type_list_filters.hpp"
#include "machine_object_holder_tuple.hpp"
#include "tlu.hpp"
#include "../submachine_conf.hpp"
#include "../states.hpp"
#include <type_traits>
#include <exception>

namespace maki::detail
{

namespace
{
//    template<class StateList, class State>
//    struct index_of_state
//    {
//        static constexpr auto value = tlu::index_of_v<StateList, State>;
//    };
//
//    template<class StateList>
//    struct index_of_state<StateList, states::stopped>
//    {
//        static constexpr auto value = -1;
//    };
//
//    template<class StateList, class State>
//    inline constexpr auto index_of_state_v = index_of_state<StateList, State>::value;

    template<class State>
    auto& state_def_of(State& state)
    {
        if constexpr(state_traits::is_submachine_v<State>)
        {
            return state.def();
        }
        else
        {
            return state;
        }
    }
}

template<class ParentSm, int Index>
class region;

template<class ParentSm, int Index>
struct region_path_of<region<ParentSm, Index>>
{
    using type = typename region_path_of_t<ParentSm>::template add<typename ParentSm::def_type, Index>;
};

template<class ParentSm, int Index>
struct machine_of<region<ParentSm, Index>>
{
    using type = root_sm_of_t<ParentSm>;
};

template<class ParentSm, int Index>
class region
{
public:
    using parent_sm_type = ParentSm;

    explicit region(ParentSm& parent_sm):
        root_sm_(machine_of<ParentSm>::get(parent_sm)),
        ctx_(parent_sm.context())/*,
        state_holders_(root_sm_, ctx_)*/
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<class StateRegionPath, class StateDef>
    const StateDef& state_def() const
    {
        if constexpr(tlu::size_v<StateRegionPath> == 0)
        {
            return state_def_of(state_from_state_def<StateDef>());
        }
        else
        {
            using submachine_t = typename tlu::front_t<StateRegionPath>::machine_def_type;
            const auto& state = state_from_state_def<submachine_t>();
            return state.template state_def<StateRegionPath, StateDef>();
        }
    }

    template<class StateRegionPath, class StateDef>
    StateDef& state_def()
    {
        if constexpr(tlu::size_v<StateRegionPath> == 0)
        {
            return state_def_of(state_from_state_def<StateDef>());
        }
        else
        {
            using submachine_t = typename tlu::front_t<StateRegionPath>::machine_def_type;
            auto& state = state_from_state_def<submachine_t>();
            return state.template state_def<StateRegionPath, StateDef>();
        }
    }

    template<class StateRelativeRegionPath, class StateDef>
    [[nodiscard]] bool is_active_state_def(const StateDef& stt) const
    {
        if constexpr(tlu::size_v<StateRelativeRegionPath> == 0)
        {
            return is_active_state_def(stt);
        }
        else
        {
            using submachine_t = typename tlu::front_t<StateRelativeRegionPath>::machine_def_type;
            const auto& state = state_from_state_def<submachine_t>();
            return state.template is_active_state_def<StateRelativeRegionPath, StateDef>();
        }
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def(const StateDef& stt) const
    {
        //if constexpr(is_type_pattern_v<StateDef>)
        //{
        //    return does_active_state_def_match_pattern<StateDef>();
        //}
        //else
        {
            return is_active_state_def_type(stt);
        }
    }

    template<class Event>
    void start(const Event& event)
    {
        if(is_active_state_def_type(states::stopped))
        {
            process_event_in_transition(states::stopped, event, initial_state, noop);
        }
    }

    template<class Event>
    void stop(const Event& event)
    {
        if(!is_active_state_def_type(states::stopped))
        {
            with_active_state_def<stop_2>
            (
                *this,
                event
            );
        }
    }

    template<class Event>
    void process_event(const Event& event)
    {
        //List the transitions whose event type pattern matches Event
        using candidate_transition_type_list = transition_table_filters::by_event_t
        <
            transition_table_type,
            Event
        >;

//        //List the state types that require us to call their on_event()
//        using candidate_state_type_list =
//            state_type_list_filters::by_required_on_event_t
//            <
//                state_type_list,
//                region,
//                Event
//            >
//        ;
//
        constexpr auto must_try_processing_event_in_transitions = !tlu::empty_v<candidate_transition_type_list>;
//        constexpr auto must_try_processing_event_in_active_state = !tlu::empty_v<candidate_state_type_list>;
//
//        if constexpr(must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
//        {
//            if(!try_processing_event_in_transitions<candidate_transition_type_list>(event))
//            {
//                try_processing_event_in_active_state<candidate_state_type_list>(event);
//            }
//        }
//        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
//        {
//            try_processing_event_in_active_state<candidate_state_type_list>(event);
//        }
//        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
//        {
//            try_processing_event_in_transitions<candidate_transition_type_list>(event);
//        }

        if constexpr(must_try_processing_event_in_transitions)
        {
            if(!try_processing_event_in_transitions<candidate_transition_type_list>(event))
            {
                try_processing_event_in_active_state<transition_table_type>(event);
            }
        }
        else
        {
            try_processing_event_in_active_state<transition_table_type>(event);
        }
    }

    template<class Event>
    void process_event(const Event& event, bool& processed)
    {
//        //List the transitions whose event type pattern matches Event
//        using candidate_transition_type_list = transition_table_filters::by_event_t
//        <
//            transition_table_type,
//            Event
//        >;
//
//        //List the state types that require us to call their on_event()
//        using candidate_state_type_list =
//            state_type_list_filters::by_required_on_event_t
//            <
//                state_type_list,
//                region,
//                Event
//            >
//        ;
//
//        constexpr auto must_try_processing_event_in_transitions = !tlu::empty_v<candidate_transition_type_list>;
//        constexpr auto must_try_processing_event_in_active_state = !tlu::empty_v<candidate_state_type_list>;
//
//        if constexpr(must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
//        {
//            if(try_processing_event_in_transitions<candidate_transition_type_list>(event))
//            {
//                processed = true;
//            }
//            else
//            {
//                try_processing_event_in_active_state<candidate_state_type_list>(event, processed);
//            }
//        }
//        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
//        {
//            try_processing_event_in_active_state<candidate_state_type_list>(event, processed);
//        }
//        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
//        {
//            try_processing_event_in_transitions<candidate_transition_type_list>(event, processed);
//        }

        if(try_processing_event_in_transitions<transition_table_type>(event))
        {
            processed = true;
        }
        else
        {
            try_processing_event_in_active_state<transition_table_type>(event, processed);
        }
    }

private:
    using root_sm_type = root_sm_of_t<ParentSm>;
    using machine_conf_tpl = typename root_sm_type::conf;

    using transition_table_type = tlu::get_t<typename ParentSm::transition_table_type_list, Index>;

    using transition_table_digest_type =
        detail::transition_table_digest<transition_table_type>
    ;

//    using transition_table_digest_type =
//        detail::transition_table_digest<transition_table_type, region>
//    ;
//
//    using state_def_type_list = typename transition_table_digest_type::state_def_type_list;
//
//    using state_type_list = typename transition_table_digest_type::state_type_list;
//
//    using non_empty_state_type_list = tlu::filter_t
//    <
//        state_type_list,
//        state_traits::needs_unique_instance
//    >;
//    using state_holder_tuple_type = tlu::apply_t<non_empty_state_type_list, machine_object_holder_tuple_t>;

//    using initial_state_def_type = detail::tlu::front_t<state_def_type_list>;

    static constexpr const auto& initial_state = tlu::front_t<transition_table_type>::source_state;

    static constexpr auto state_ptrs = transition_table_digest<transition_table_type>::states;

    struct stop_2
    {
        template<class Event, class ActiveState>
        static void call(const ActiveState& stt, region& self, const Event& event)
        {
            self.process_event_in_transition(stt, event, states::stopped, noop);
        }
    };

    template<class TransitionTypeList, class Event, class... ExtraArgs>
    bool try_processing_event_in_transitions(const Event& event, ExtraArgs&... extra_args)
    {
        return tlu::for_each_or
        <
            TransitionTypeList,
            try_processing_event_in_transition
        >(*this, event, extra_args...);
    }

    //Check active state and guard
    struct try_processing_event_in_transition
    {
        template<class Transition, class Event, class... ExtraArgs>
        static bool call(region& self, const Event& event, ExtraArgs&... extra_args)
        {
            static constexpr const auto& source_state = Transition::source_state;
            static constexpr const auto& target_state = Transition::target_state;

            //if constexpr(is_type_pattern_v<source_state_t>)
            //{
            //    //List of state defs that match with the source state pattern
            //    using matching_state_def_type_list = state_type_list_filters::by_pattern_t
            //    <
            //        state_def_type_list,
            //        source_state_t
            //    >;

            //    static_assert(!tlu::empty_v<matching_state_def_type_list>);

            //    return tlu::for_each_or
            //    <
            //        matching_state_def_type_list,
            //        try_processing_event_in_transition_2
            //        <
            //            target_state_t,
            //            Transition::action,
            //            Transition::guard
            //        >
            //    >(self, event, extra_args...);
            //}
            //else
            {
                return try_processing_event_in_transition_2
                <
                    Transition::action,
                    Transition::guard
                >::template call(self, source_state, event, target_state, extra_args...);
            }
        }
    };

    template<const auto& Action, const auto& Guard>
    struct try_processing_event_in_transition_2
    {
        template<class SourceState, class Event, class TargetState, class... ExtraArgs>
        static bool call
        (
            region& self,
            const SourceState& source_state,
            const Event& event,
            const TargetState& target_state,
            ExtraArgs&... extra_args
        )
        {
            //Make sure the transition source state is the active state
            if(!self.is_active_state_def_type(source_state))
            {
                return false;
            }

            //Check guard
            if(!detail::call_action_or_guard(Guard, self.root_sm_, self.ctx_, event))
            {
                return false;
            }

            self.process_event_in_transition
            (
                source_state,
                event,
                target_state,
                Action,
                extra_args...
            );

            return true;
        }
    };

    template<class SourceState, class Event, class TargetState, class Action>
    void process_event_in_transition
    (
        const SourceState& source_state,
        const Event& event,
        const TargetState& target_state,
        const Action& action,
        bool& processed
    )
    {
        process_event_in_transition(source_state, event, target_state, action);
        processed = true;
    }

    template<class SourceState, class Event, class TargetState, class Action>
    void process_event_in_transition
    (
        const SourceState& source_state,
        const Event& event,
        const TargetState& target_state,
        const Action& action
    )
    {
        using path_t = region_path_of_t<region>;

        const auto is_internal_transition = std::is_same_v<TargetState, null_t>;

        if constexpr(!is_internal_transition)
        {
            if constexpr(option_v<machine_conf_tpl, option_id::before_state_transition>)
            {
                root_sm_.def().template before_state_transition<path_t>
                (
                    source_state,
                    event,
                    target_state
                );
            }

            detail::call_mce
            (
                source_state.on_exit,
                root_sm_,
                ctx_,
                event
            );

            pactive_state_ = &target_state;
        }

        detail::call_action_or_guard
        (
            action,
            root_sm_,
            ctx_,
            event
        );

        if constexpr(!is_internal_transition)
        {
            detail::call_mce
            (
                target_state.on_entry,
                root_sm_,
                ctx_,
                event
            );

            if constexpr(option_v<machine_conf_tpl, option_id::after_state_transition>)
            {
                root_sm_.def().template after_state_transition
                <
                    path_t
                >(source_state, event, target_state);
            }

            //Anonymous transition
            //if constexpr(transition_table_digest_type::has_null_events)
            {
                using candidate_transition_type_list = transition_table_filters::by_event_t
                <
                    transition_table_type,
                    null_t
                >;

                try_processing_event_in_transitions<candidate_transition_type_list>(null);
            }
        }
    }

    /*
    Call active_state.on_event(event)
    */
    template<class StateTypeList, class Event, class... ExtraArgs>
    void try_processing_event_in_active_state(const Event& event, ExtraArgs&... extra_args)
    {
        const auto try_processing = [](region& self, const auto& stt, const Event& event, ExtraArgs&... /*extra_args*/)
        {
            if(!self.is_active_state_type(stt))
            {
                return false;
            }

            if constexpr(!std::is_same_v<std::decay_t<decltype(stt)>, null_t>)
            {
                call_mce(stt.on_event, self.root_sm_, self.ctx_, event);
            }

            return true;
        };

        apply
        (
            state_ptrs,
            [](region& self, const auto& try_processing, const auto& event, /*auto&... extra_args,*/ const auto... pstates)
            {
                (
                    try_processing(self, *pstates, event/*, extra_args...*/) ||
                    ...
                );
            },
            *this,
            try_processing,
            event/*,
            extra_args...*/
        );
    }

    template<class State>
    [[nodiscard]] bool is_active_state_type(const State& stt) const
    {
        return &stt == pactive_state_;
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def_type(const StateDef& stt) const
    {
        return &stt == pactive_state_;
    }

    //template<class TypePattern>
    //[[nodiscard]] bool does_active_state_def_match_pattern() const
    //{
    //    auto matches = false;
    //    with_active_state_def
    //    <
    //        tlu::push_back_t<state_def_type_list, states::stopped>,
    //        does_active_state_def_match_pattern_2<TypePattern>
    //    >(matches);
    //    return matches;
    //}

    template<class TypePattern>
    struct does_active_state_def_match_pattern_2
    {
        template<class ActiveState>
        static void call([[maybe_unused]] bool& matches)
        {
            if constexpr(matches_pattern_v<ActiveState, TypePattern>)
            {
                matches = true;
            }
        }
    };

    template<class F, class... Args>
    void with_active_state_def(Args&&... args) const
    {
        for_each_element_or
        (
            state_ptrs,
            [](const auto pstate, const auto pactive_state, auto&&... args)
            {
                if(pactive_state == pstate)
                {
                    F::call
                    (
                        *pstate,
                        std::forward<decltype(args)>(args)...
                    );
                    return true;
                }
                return false;
            },
            pactive_state_,
            std::forward<Args>(args)...
        );
    }

    template<class StateDef>
    auto& state_from_state_def()
    {
        using state_t = state_traits::state_def_to_state_t<StateDef, region>;
        return state<state_t>();
    }

    template<class StateDef>
    const auto& state_from_state_def() const
    {
        using state_t = state_traits::state_def_to_state_t<StateDef, region>;
        return state<state_t>();
    }

    template<class State>
    auto& state()
    {
        return static_state<State>(*this);
    }

    template<class State>
    const auto& state() const
    {
        return static_state<State>(*this);
    }

    template<class State, class Self>
    static auto& static_state(Self& reg)
    {
        if constexpr(state_traits::needs_unique_instance<State>::value)
        {
            return get<machine_object_holder<State>>(reg.state_holders_).get();
        }
        else
        {
            //Optimize empty state case by returning a statically allocated
            //instance.
            return static_instance<State>;
        }
    }

    template<class T>
    static T static_instance; //NOLINT

    root_sm_type& root_sm_;
    std::decay_t<typename ParentSm::context_type>& ctx_;
    //state_holder_tuple_type state_holders_;

    //int active_state_index_ = index_of_state_v<state_def_type_list, states::stopped>;
    const void* pactive_state_ = &states::stopped;
};

template<class ParentSm, int Index>
template<class T>
T region<ParentSm, Index>::static_instance; //NOLINT

} //namespace

#endif

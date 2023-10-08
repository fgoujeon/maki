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
    template<class StateList, class State>
    struct index_of_state
    {
        static constexpr auto value = tlu::index_of_v<StateList, State>;
    };

    template<class StateList>
    struct index_of_state<StateList, states::stopped>
    {
        static constexpr auto value = -1;
    };

    template<class StateList, class State>
    inline constexpr auto index_of_state_v = index_of_state<StateList, State>::value;

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
    static constexpr auto value = region_path_of_v<ParentSm>.template add<typename ParentSm::def_type, Index>();
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
        ctx_(parent_sm.context()),
        state_holders_(root_sm_, ctx_)
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<const auto& StateRegionPath, class StateDef>
    const StateDef& state_def() const
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        if constexpr(tlu::size_v<state_region_path_t> == 0)
        {
            return state_def_of(state_from_state_def<StateDef>());
        }
        else
        {
            using submachine_t = typename tlu::front_t<state_region_path_t>::machine_def_type;
            const auto& state = state_from_state_def<submachine_t>();
            return state.template state_def<StateRegionPath, StateDef>();
        }
    }

    template<const auto& StateRegionPath, class StateDef>
    StateDef& state_def()
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        if constexpr(tlu::size_v<state_region_path_t> == 0)
        {
            return state_def_of(state_from_state_def<StateDef>());
        }
        else
        {
            using submachine_t = typename tlu::front_t<state_region_path_t>::machine_def_type;
            auto& state = state_from_state_def<submachine_t>();
            return state.template state_def<StateRegionPath, StateDef>();
        }
    }

    template<const auto& StateRelativeRegionPath, class StateDef>
    [[nodiscard]] bool is_active_state_def() const
    {
        using state_relative_region_path_t = std::decay_t<decltype(StateRelativeRegionPath)>;

        if constexpr(tlu::size_v<state_relative_region_path_t> == 0)
        {
            return is_active_state_def<StateDef>();
        }
        else
        {
            using submachine_t = typename tlu::front_t<state_relative_region_path_t>::machine_def_type;
            const auto& state = state_from_state_def<submachine_t>();
            return state.template is_active_state_def<StateRelativeRegionPath, StateDef>();
        }
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def() const
    {
        if constexpr(is_type_pattern_v<StateDef>)
        {
            return does_active_state_def_match_pattern<StateDef>();
        }
        else
        {
            return is_active_state_def_type<StateDef>();
        }
    }

    template<class Event>
    void start(const Event& event)
    {
        if(is_active_state_def_type<states::stopped>())
        {
            process_event_in_transition<states::stopped, initial_state_def_type, noop>(event);
        }
    }

    template<class Event>
    void stop(const Event& event)
    {
        if(!is_active_state_def_type<states::stopped>())
        {
            with_active_state_def<state_def_type_list, stop_2>
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

        //List the state types that require us to call their on_event()
        using candidate_state_type_list =
            state_type_list_filters::by_required_on_event_t
            <
                state_type_list,
                region,
                Event
            >
        ;

        constexpr auto must_try_processing_event_in_transitions = !tlu::empty_v<candidate_transition_type_list>;
        constexpr auto must_try_processing_event_in_active_state = !tlu::empty_v<candidate_state_type_list>;

        if constexpr(must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            if(!try_processing_event_in_transitions<candidate_transition_type_list>(event))
            {
                try_processing_event_in_active_state<candidate_state_type_list>(event);
            }
        }
        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            try_processing_event_in_active_state<candidate_state_type_list>(event);
        }
        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
        {
            try_processing_event_in_transitions<candidate_transition_type_list>(event);
        }
    }

    template<class Event>
    void process_event(const Event& event, bool& processed)
    {
        //List the transitions whose event type pattern matches Event
        using candidate_transition_type_list = transition_table_filters::by_event_t
        <
            transition_table_type,
            Event
        >;

        //List the state types that require us to call their on_event()
        using candidate_state_type_list =
            state_type_list_filters::by_required_on_event_t
            <
                state_type_list,
                region,
                Event
            >
        ;

        constexpr auto must_try_processing_event_in_transitions = !tlu::empty_v<candidate_transition_type_list>;
        constexpr auto must_try_processing_event_in_active_state = !tlu::empty_v<candidate_state_type_list>;

        if constexpr(must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            if(try_processing_event_in_transitions<candidate_transition_type_list>(event))
            {
                processed = true;
            }
            else
            {
                try_processing_event_in_active_state<candidate_state_type_list>(event, processed);
            }
        }
        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            try_processing_event_in_active_state<candidate_state_type_list>(event, processed);
        }
        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
        {
            try_processing_event_in_transitions<candidate_transition_type_list>(event, processed);
        }
    }

private:
    using root_sm_type = root_sm_of_t<ParentSm>;
    static constexpr auto machine_conf = root_sm_type::conf;

    using transition_table_type = tlu::get_t<typename ParentSm::transition_table_type_list, Index>;

    using transition_table_digest_type =
        detail::transition_table_digest<transition_table_type, region>
    ;

    using state_def_type_list = typename transition_table_digest_type::state_def_type_list;

    using state_type_list = typename transition_table_digest_type::state_type_list;

    using non_empty_state_type_list = tlu::filter_t
    <
        state_type_list,
        state_traits::needs_unique_instance
    >;
    using state_holder_tuple_type = tlu::apply_t<non_empty_state_type_list, machine_object_holder_tuple_t>;

    using initial_state_def_type = detail::tlu::front_t<state_def_type_list>;

    struct stop_2
    {
        template<class ActiveState, class Event>
        static void call(region& self, const Event& event)
        {
            self.process_event_in_transition
            <
                ActiveState,
                states::stopped,
                noop
            >(event);
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
            using source_state_t = typename Transition::source_state_type_pattern;
            using target_state_t = typename Transition::target_state_type;

            if constexpr(is_type_pattern_v<source_state_t>)
            {
                //List of state defs that match with the source state pattern
                using matching_state_def_type_list = state_type_list_filters::by_pattern_t
                <
                    state_def_type_list,
                    source_state_t
                >;

                static_assert(!tlu::empty_v<matching_state_def_type_list>);

                return tlu::for_each_or
                <
                    matching_state_def_type_list,
                    try_processing_event_in_transition_2
                    <
                        target_state_t,
                        Transition::action,
                        Transition::guard
                    >
                >(self, event, extra_args...);
            }
            else
            {
                return try_processing_event_in_transition_2
                <
                    target_state_t,
                    Transition::action,
                    Transition::guard
                >::template call<source_state_t>(self, event, extra_args...);
            }
        }
    };

    template<class TargetStateDef, const auto& Action, const auto& Guard>
    struct try_processing_event_in_transition_2
    {
        template<class SourceStateDef, class Event, class... ExtraArgs>
        static bool call(region& self, const Event& event, ExtraArgs&... extra_args)
        {
            //Make sure the transition source state is the active state
            if(!self.is_active_state_def_type<SourceStateDef>())
            {
                return false;
            }

            //Check guard
            if(!detail::call_action_or_guard<Guard>(self.root_sm_, self.ctx_, event))
            {
                return false;
            }

            self.process_event_in_transition
            <
                SourceStateDef,
                TargetStateDef,
                Action
            >(event, extra_args...);

            return true;
        }
    };

    template<class SourceStateDef, class TargetStateDef, const auto& Action, class Event>
    void process_event_in_transition(const Event& event, bool& processed)
    {
        process_event_in_transition<SourceStateDef, TargetStateDef, Action>(event);
        processed = true;
    }

    template<class SourceStateDef, class TargetStateDef, const auto& Action, class Event>
    void process_event_in_transition(const Event& event)
    {
        constexpr const auto& path = region_path_of_v<region>;

        constexpr auto is_internal_transition =
            std::is_same_v<TargetStateDef, null>
        ;

        if constexpr(!is_internal_transition)
        {
            if constexpr(machine_conf.has_before_state_transition)
            {
                root_sm_.def().template before_state_transition
                <
                    path,
                    SourceStateDef,
                    Event,
                    TargetStateDef
                >(event);
            }

            if constexpr(!std::is_same_v<SourceStateDef, states::stopped>)
            {
                detail::call_on_exit
                (
                    state_from_state_def<SourceStateDef>(),
                    root_sm_,
                    event
                );
            }

            active_state_index_ = index_of_state_v
            <
                state_def_type_list,
                TargetStateDef
            >;
        }

        detail::call_action_or_guard<Action>
        (
            root_sm_,
            ctx_,
            event
        );

        if constexpr(!is_internal_transition)
        {
            if constexpr(!std::is_same_v<TargetStateDef, states::stopped>)
            {
                detail::call_on_entry
                (
                    state_from_state_def<TargetStateDef>(),
                    root_sm_,
                    event
                );
            }

            if constexpr(machine_conf.has_after_state_transition)
            {
                root_sm_.def().template after_state_transition
                <
                    path,
                    SourceStateDef,
                    Event,
                    TargetStateDef
                >(event);
            }

            //Anonymous transition
            if constexpr(transition_table_digest_type::has_null_events)
            {
                using candidate_transition_type_list = transition_table_filters::by_event_t
                <
                    transition_table_type,
                    null
                >;

                try_processing_event_in_transitions<candidate_transition_type_list>(null{});
            }
        }
    }

    /*
    Call active_state.on_event(event)
    */
    template<class StateTypeList, class Event, class... ExtraArgs>
    void try_processing_event_in_active_state(const Event& event, ExtraArgs&... extra_args)
    {
        tlu::for_each_or
        <
            StateTypeList,
            try_processing_event_in_active_state_2
        >(*this, event, extra_args...);
    }

    struct try_processing_event_in_active_state_2
    {
        template<class State, class Event, class... ExtraArgs>
        static bool call(region& self, const Event& event, ExtraArgs&... extra_args)
        {
            if(!self.is_active_state_type<State>())
            {
                return false;
            }

            auto& state = self.state<State>();
            call_on_event(state, self.root_sm_, self.ctx_, event, extra_args...);
            return true;
        }
    };

    template<class State>
    [[nodiscard]] bool is_active_state_type() const
    {
        constexpr auto given_state_index = index_of_state_v
        <
            state_type_list,
            State
        >;
        return given_state_index == active_state_index_;
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def_type() const
    {
        constexpr auto given_state_index = index_of_state_v
        <
            state_def_type_list,
            StateDef
        >;
        return given_state_index == active_state_index_;
    }

    template<class TypePattern>
    [[nodiscard]] bool does_active_state_def_match_pattern() const
    {
        auto matches = false;
        with_active_state_def
        <
            tlu::push_back_t<state_def_type_list, states::stopped>,
            does_active_state_def_match_pattern_2<TypePattern>
        >(matches);
        return matches;
    }

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

    template<class StateDefTypeList, class F, class... Args>
    void with_active_state_def(Args&&... args) const
    {
        tlu::for_each_or
        <
            StateDefTypeList,
            with_active_state_def_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_def_2
    {
        template<class StateDef, class... Args>
        static bool call(const region& self, Args&&... args)
        {
            if(self.is_active_state_def_type<StateDef>())
            {
                F::template call<StateDef>(std::forward<Args>(args)...);
                return true;
            }
            return false;
        }
    };

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

    //Store references for faster access
    root_sm_type& root_sm_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    std::decay_t<typename ParentSm::context_type>& ctx_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    state_holder_tuple_type state_holders_;

    int active_state_index_ = index_of_state_v<state_def_type_list, states::stopped>;
};

template<class ParentSm, int Index>
template<class T>
T region<ParentSm, Index>::static_instance; //NOLINT

} //namespace

#endif

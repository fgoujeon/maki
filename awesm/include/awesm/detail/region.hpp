//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "region_path_of.hpp"
#include "state_traits.hpp"
#include "call_member.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "state_type_list_filters.hpp"
#include "tlu.hpp"
#include "../subsm_conf.hpp"
#include "../states.hpp"
#include <type_traits>
#include <exception>

namespace awesm::detail
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
        if constexpr(state_traits::is_subsm_v<State>)
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
struct root_sm_of<region<ParentSm, Index>>
{
    using type = root_sm_of_t<ParentSm>;
};

template<class State>
struct call_state_on_event_t
{
    template<class Region, class Event>
    auto operator()
    (
        [[maybe_unused]] Region& reg,
        [[maybe_unused]] const Event& event
    ) const
    {
        return reg.template state<State>().on_event(event);
    }
};

template<class State>
inline constexpr auto call_state_on_event = call_state_on_event_t<State>{};

template<class TransitionTable, class State>
struct add_internal_transition
{
    template<class Event>
    struct requires_on_event
    {
        static constexpr auto value = state_traits::requires_on_event_v<State, Event>;
    };

    using transition_type = awesm::transition
    <
        State,
        any_if<requires_on_event>,
        awesm::null,
        call_state_on_event<State>
    >;

    using type = tlu::push_back_if_t
    <
        TransitionTable,
        transition_type,
        state_traits::requires_on_event_for_at_least_one_event_v<State>
    >;
};

template<class TransitionTable, class State>
using add_internal_transition_t = typename add_internal_transition<TransitionTable, State>::type;

template<class ParentSm, int Index>
class region
{
public:
    using parent_sm_type = ParentSm;

    explicit region(ParentSm& parent_sm):
        root_sm_(root_sm_of<ParentSm>::get(parent_sm)),
        ctx_(parent_sm.context()),
        state_holders_(root_sm_, ctx_)
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<class State>
    State& state()
    {
        return get<sm_object_holder<State>>(state_holders_).get();
    }

    template<class StateRegionPath, class StateDef>
    const StateDef& state_def() const
    {
        if constexpr(tlu::size_v<StateRegionPath> == 0)
        {
            return state_def_of(state_from_state_def<StateDef>());
        }
        else
        {
            using subsm_t = typename tlu::front_t<StateRegionPath>::sm_def_type;
            const auto& state = state_from_state_def<subsm_t>();
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
            using subsm_t = typename tlu::front_t<StateRegionPath>::sm_def_type;
            auto& state = state_from_state_def<subsm_t>();
            return state.template state_def<StateRegionPath, StateDef>();
        }
    }

    template<class StateRelativeRegionPath, class StateDef>
    [[nodiscard]] bool is_active_state_def() const
    {
        if constexpr(tlu::size_v<StateRelativeRegionPath> == 0)
        {
            return is_active_state_def<StateDef>();
        }
        else
        {
            using subsm_t = typename tlu::front_t<StateRelativeRegionPath>::sm_def_type;
            const auto& state = state_from_state_def<subsm_t>();
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
        if(is_active_state_type<states::stopped>())
        {
            process_event_in_transition<states::stopped, initial_state_type, noop>(event);
        }
    }

    template<class Event>
    void stop(const Event& event)
    {
        if(!is_active_state_type<states::stopped>())
        {
            with_active_state<state_type_list, stop_2>
            (
                *this,
                event
            );
        }
    }

    template<class Event>
    bool process_event(const Event& event)
    {
        //List the transitions whose event type pattern matches Event
        using candidate_transition_type_list = transition_table_filters::by_event_t
        <
            extended_transition_table_type,
            Event
        >;

        return try_processing_event_in_transitions<candidate_transition_type_list>(event);
    }

private:
    using root_sm_type = root_sm_of_t<ParentSm>;
    using sm_conf_tpl = typename root_sm_type::conf;

    //Transition table as given by client
    using transition_table_type = tlu::get_t<typename ParentSm::transition_table_type_list, Index>;

    using transition_table_digest_type =
        detail::transition_table_digest<transition_table_type, region>
    ;

    //Transition table with state types instead of state def types
    using resolved_transition_table_type = typename transition_table_digest_type::resolved_transition_table;

    using state_def_type_list = typename transition_table_digest_type::state_def_type_list;

    using state_type_list = typename transition_table_digest_type::state_type_list;

    using state_holder_tuple_type =
        typename transition_table_digest_type::state_holder_tuple_type
    ;

    //resolved_transition_table_type + internal transitions provided by the
    //on_event() state functions
    using extended_transition_table_type = tlu::left_fold_t
    <
        state_type_list,
        add_internal_transition_t,
        resolved_transition_table_type
    >;

    using initial_state_type = detail::tlu::front_t<state_type_list>;

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

    template<class TransitionTypeList, class Event>
    bool try_processing_event_in_transitions(const Event& event)
    {
        auto processed = false;
        tlu::for_each_or
        <
            TransitionTypeList,
            try_processing_event_in_transition
        >(*this, event, processed);
        return processed;
    }

    //Check active state and guard
    struct try_processing_event_in_transition
    {
        template<class Transition, class Event>
        static bool call(region& self, const Event& event, bool& processed)
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

                using matching_state_type_list = state_traits::state_def_type_list_to_state_type_list_t
                <
                    matching_state_def_type_list,
                    region
                >;

                static_assert(!tlu::empty_v<matching_state_def_type_list>);

                return tlu::for_each_or
                <
                    matching_state_type_list,
                    try_processing_event_in_transition_2
                    <
                        target_state_t,
                        Transition::action,
                        Transition::guard
                    >
                >(self, event, processed);
            }
            else
            {
                return try_processing_event_in_transition_2
                <
                    target_state_t,
                    Transition::action,
                    Transition::guard
                >::template call<source_state_t>(self, event, processed);
            }
        }
    };

    template<class TargetState, const auto& Action, const auto& Guard>
    struct try_processing_event_in_transition_2
    {
        template<class SourceState, class Event>
        static bool call(region& self, const Event& event, bool& processed)
        {
            //Make sure the transition source state is the active state
            if(!self.is_active_state_type<SourceState>())
            {
                return false;
            }

            //Check guard
            if(!detail::call_action_or_guard<Guard>(self.root_sm_, self, self.ctx_, event))
            {
                return false;
            }

            processed = self.process_event_in_transition
            <
                SourceState,
                TargetState,
                Action
            >(event);
            return true;
        }
    };

    template<class SourceState, class TargetState, const auto& Action, class Event>
    bool process_event_in_transition(const Event& event)
    {
        using path_t = region_path_of_t<region>;

        using source_state_def_t = state_traits::state_to_state_def_t<SourceState>;
        using target_state_def_t = state_traits::state_to_state_def_t<TargetState>;

        constexpr auto is_internal_transition =
            std::is_same_v<TargetState, null>
        ;

        if constexpr(!is_internal_transition)
        {
            if constexpr(option_v<sm_conf_tpl, option_id::before_state_transition>)
            {
                root_sm_.def().template before_state_transition
                <
                    path_t,
                    source_state_def_t,
                    Event,
                    target_state_def_t
                >(event);
            }

            if constexpr(!std::is_same_v<SourceState, states::stopped>)
            {
                detail::call_on_exit
                (
                    state<SourceState>(),
                    root_sm_,
                    event
                );
            }

            active_state_index_ = index_of_state_v
            <
                state_type_list,
                TargetState
            >;
        }

        const auto call_action = [&]
        {
            return detail::call_action_or_guard<Action>
            (
                root_sm_,
                *this,
                ctx_,
                event
            );
        };

        using call_action_return_t = decltype(call_action());

        const auto processed = [&]
        {
            if constexpr(std::is_void_v<call_action_return_t>)
            {
                detail::call_action_or_guard<Action>
                (
                    root_sm_,
                    *this,
                    ctx_,
                    event
                );
                return true;
            }
            else
            {
                return detail::call_action_or_guard<Action>
                (
                    root_sm_,
                    *this,
                    ctx_,
                    event
                );
            }
        }();

        if constexpr(!is_internal_transition)
        {
            if constexpr(!std::is_same_v<TargetState, states::stopped>)
            {
                detail::call_on_entry
                (
                    state<TargetState>(),
                    root_sm_,
                    event
                );
            }

            if constexpr(option_v<sm_conf_tpl, option_id::after_state_transition>)
            {
                root_sm_.def().template after_state_transition
                <
                    path_t,
                    source_state_def_t,
                    Event,
                    target_state_def_t
                >(event);
            }

            //Anonymous transition
            if constexpr(transition_table_digest_type::has_null_events)
            {
                using candidate_transition_type_list = transition_table_filters::by_event_t
                <
                    resolved_transition_table_type,
                    null
                >;

                try_processing_event_in_transitions<candidate_transition_type_list>(null{});
            }
        }

        return processed;
    }

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

    template<class StateTypeList, class F, class... Args>
    void with_active_state(Args&&... args) const
    {
        tlu::for_each_or
        <
            StateTypeList,
            with_active_state_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_2
    {
        template<class State, class... Args>
        static bool call(const region& self, Args&&... args)
        {
            if(self.is_active_state_type<State>())
            {
                F::template call<State>(std::forward<Args>(args)...);
                return true;
            }
            return false;
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
        return get<sm_object_holder<state_t>>(state_holders_).get();
    }

    template<class StateDef>
    const auto& state_from_state_def() const
    {
        using state_t = state_traits::state_def_to_state_t<StateDef, region>;
        return get<sm_object_holder<state_t>>(state_holders_).get();
    }

    root_sm_type& root_sm_;
    std::decay_t<typename ParentSm::context_type>& ctx_;
    state_holder_tuple_type state_holders_;

    int active_state_index_ = index_of_state_v<state_type_list, states::stopped>;
};

} //namespace

#endif

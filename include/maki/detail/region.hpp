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
#include "constant.hpp"
#include "../submachine_conf.hpp"
#include "../states.hpp"
#include <type_traits>
#include <exception>

namespace maki::detail
{

namespace region_detail
{
    template<class StateList, class State>
    struct index_of_state
    {
        static constexpr auto value = tlu::index_of_v<StateList, State>;
    };

    template<class StateList>
    struct index_of_state<StateList, state_conf_wrapper<states::stopped>>
    {
        static constexpr auto value = -1;
    };

    template<class StateList, class State>
    inline constexpr auto index_of_state_v = index_of_state<StateList, State>::value;
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
        state_data_holders_(uniform_construct, root_sm_, ctx_)
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<const auto& StateRegionPath, class StateDef>
    const auto& state_def_data() const
    {
        return static_state_def_data<StateRegionPath, StateDef>(*this);
    }

    template<const auto& StateRegionPath, class StateDef>
    auto& state_def_data()
    {
        return static_state_def_data<StateRegionPath, StateDef>(*this);
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
            return does_active_state_def_match_pattern
            <
                std::decay_t<decltype(StateDef::conf)>
            >();
        }
        else
        {
            return is_active_state_def_type<StateDef>();
        }
    }

    template<class Event>
    void start(const Event& event)
    {
        if(is_active_state_def_type<state_conf_wrapper<states::stopped>>())
        {
            process_event_in_transition<state_conf_wrapper<states::stopped>, initial_state_def_type, noop>(event);
        }
    }

    template<class Event>
    void stop(const Event& event)
    {
        if(!is_active_state_def_type<state_conf_wrapper<states::stopped>>())
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
    using machine_conf_type = std::decay_t<decltype(machine_conf)>;

    using transition_table_type = tlu::get_t<typename ParentSm::transition_table_type_list, Index>;

    using transition_table_digest_type =
        detail::transition_table_digest<transition_table_type, region>
    ;

    using state_def_type_list = typename transition_table_digest_type::state_def_type_list;
    using state_type_list = typename transition_table_digest_type::state_type_list;

    using state_data_type_list = typename transition_table_digest_type::state_data_type_list;
    using state_data_holder_tuple_type = tlu::apply_t<state_data_type_list, machine_object_holder_tuple_t>;

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
                state_conf_wrapper<states::stopped>,
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
                    std::decay_t<decltype(source_state_t::conf)>
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
            std::is_same_v<TargetStateDef, state_conf_wrapper<null_c>>
        ;

        if constexpr(!is_internal_transition)
        {
            if constexpr(!std::is_same_v<typename machine_conf_type::pre_state_transition_action_type, noop_ex>)
            {
                machine_conf.pre_state_transition_action_
                (
                    ctx_,
                    constant_c<path>,
                    constant_c<SourceStateDef::conf>,
                    event,
                    constant_c<TargetStateDef::conf>
                );
            }

            if constexpr(!std::is_same_v<SourceStateDef, state_conf_wrapper<states::stopped>>)
            {
                using source_state_t = state_traits::state_def_to_state_t<SourceStateDef, region>;
                detail::call_state_action
                (
                    source_state_t::conf.exit_actions,
                    root_sm_,
                    ctx_,
                    state_from_state_def<SourceStateDef>(),
                    event
                );
            }

            active_state_index_ = region_detail::index_of_state_v
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
            if constexpr(!std::is_same_v<TargetStateDef, state_conf_wrapper<states::stopped>>)
            {
                using target_state_t = state_traits::state_def_to_state_t<TargetStateDef, region>;
                detail::call_state_action
                (
                    target_state_t::conf.entry_actions,
                    root_sm_,
                    ctx_,
                    state_from_state_def<TargetStateDef>(),
                    event
                );
            }

            if constexpr(!std::is_same_v<typename machine_conf_type::post_state_transition_action_type, noop_ex>)
            {
                machine_conf.post_state_transition_action_
                (
                    ctx_,
                    constant_c<path>,
                    constant_c<SourceStateDef::conf>,
                    event,
                    constant_c<TargetStateDef::conf>
                );
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

            auto& state_data = self.state_data<State>();
            call_state_action_old
            (
                state_data,
                State::conf.event_actions,
                self.root_sm_,
                self.ctx_,
                state_data,
                event,
                extra_args...
            );
            return true;
        }
    };

    template<class State>
    [[nodiscard]] bool is_active_state_type() const
    {
        constexpr auto given_state_index = region_detail::index_of_state_v
        <
            state_type_list,
            State
        >;
        return given_state_index == active_state_index_;
    }

    template<class StateDef>
    [[nodiscard]] bool is_active_state_def_type() const
    {
        constexpr auto given_state_index = region_detail::index_of_state_v
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
            tlu::push_back_t<state_def_type_list, state_conf_wrapper<states::stopped>>,
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
        return state_data<state_t>();
    }

    template<class StateDef>
    const auto& state_from_state_def() const
    {
        using state_t = state_traits::state_def_to_state_t<StateDef, region>;
        return state_data<state_t>();
    }

    template<int StateIndex>
    auto& state_data()
    {
        return tuple_get<StateIndex>(state_data_holders_).get();
    }

    template<int StateIndex>
    const auto& state_data() const
    {
        return tuple_get<StateIndex>(state_data_holders_).get();
    }

    template<class State>
    auto& state_data()
    {
        constexpr auto state_index = tlu::index_of_v<state_type_list, State>;
        return tuple_get<state_index>(state_data_holders_).get();
    }

    template<class State>
    const auto& state_data() const
    {
        constexpr auto state_index = tlu::index_of_v<state_type_list, State>;
        return tuple_get<state_index>(state_data_holders_).get();
    }

    template<class StateDef, class Region>
    static auto& static_state_def_data_leaf(Region& self)
    {
        using conf_type = std::decay_t<decltype(StateDef::conf)>;
        if constexpr(is_submachine_conf_v<conf_type>)
        {
            using state_t = state_traits::state_def_to_state_t<StateDef, std::decay_t<Region>>;
            return self.template state_data<state_t>().def_data();
        }
        else
        {
            return self.template state_data<StateDef>();
        }
    }

    template<const auto& StateRegionPath, class StateDef, class Region>
    static auto& static_state_def_data(Region& self)
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        if constexpr(tlu::size_v<state_region_path_t> == 0)
        {
            return static_state_def_data_leaf<StateDef>(self);
        }
        else
        {
            using submachine_t = typename tlu::front_t<state_region_path_t>::machine_def_type;
            constexpr auto submachine_index = tlu::index_of_v<typename Region::state_def_type_list, submachine_t>;
            auto& submachine_data = self.template state_data<submachine_index>();
            return submachine_data.template state_def_data<StateRegionPath, StateDef>(); //recursive
        }
    }

    //Store references for faster access
    root_sm_type& root_sm_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    std::decay_t<typename ParentSm::context_type>& ctx_; //NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)

    state_data_holder_tuple_type state_data_holders_;

    int active_state_index_ = region_detail::index_of_state_v<state_def_type_list, state_conf_wrapper<states::stopped>>;
};

} //namespace

#endif

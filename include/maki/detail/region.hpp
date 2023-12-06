//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_HPP
#define MAKI_DETAIL_REGION_HPP

#include "region_path_of.hpp"
#include "machine_of.hpp"
#include "state_traits.hpp"
#include "call_member.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "state_type_list_filters.hpp"
#include "tlu.hpp"
#include "noop_ex.hpp"
#include "same_ref.hpp"
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

    template<class StateList, class Region>
    struct index_of_state<StateList, state_traits::state_conf_to_state<states::stopped, Region>>
    {
        static constexpr auto value = -1;
    };

    template<class StateList, class State>
    inline constexpr auto index_of_state_v = index_of_state<StateList, State>::value;

    template<class StateList, const auto& StateConf>
    struct index_of_state_conf
    {
        static constexpr auto value = tlu::index_of_v<StateList, constant<StateConf>>;
    };

    template<class StateList>
    struct index_of_state_conf<StateList, states::stopped>
    {
        static constexpr auto value = -1;
    };

    template<class StateList, const auto& StateConf>
    inline constexpr auto index_of_state_conf_v = index_of_state_conf<StateList, StateConf>::value;

    inline void set_to_true(bool& value)
    {
        value = true;
    }

    inline void set_to_true()
    {
        //nothing
    }
}

template<class ParentSm, int Index>
class region
{
public:
    using parent_sm_type = ParentSm;

    template<class Machine>
    region(Machine& mach, ParentSm& parent_sm):
        states_(uniform_construct, mach, parent_sm.context())
    {
    }

    region(const region&) = delete;
    region(region&&) = delete;
    region& operator=(const region&) = delete;
    region& operator=(region&&) = delete;
    ~region() = default;

    template<const auto& StateRegionPath, const auto& StateConf>
    const auto& state_def_data() const
    {
        return static_state_def_data<StateRegionPath, StateConf>(*this);
    }

    template<const auto& StateRegionPath, const auto& StateConf>
    auto& state_def_data()
    {
        return static_state_def_data<StateRegionPath, StateConf>(*this);
    }

    template<const auto& StateRelativeRegionPath, const auto& StateConf>
    [[nodiscard]] bool is_active_state_def() const
    {
        using state_relative_region_path_t = std::decay_t<decltype(StateRelativeRegionPath)>;

        if constexpr(tlu::size_v<state_relative_region_path_t> == 0)
        {
            return is_active_state_def<StateConf>();
        }
        else
        {
            constexpr const auto& submach_conf = tlu::front_t<state_relative_region_path_t>::machine_conf;
            constexpr auto submach_index = tlu::index_of_v<state_conf_constant_list, constant<submach_conf>>;
            const auto& state = tuple_get<submach_index>(states_);
            return state.template is_active_state_def<StateRelativeRegionPath, StateConf>();
        }
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is_active_state_def() const
    {
        if constexpr(is_type_pattern_v<std::decay_t<decltype(StateConf)>>)
        {
            return does_active_state_def_match_pattern
            <
                std::decay_t<decltype(StateConf)>
            >();
        }
        else
        {
            return is_active_state_def_type<StateConf>();
        }
    }

    template<class Machine, class Context, class Event>
    void start(Machine& mach, Context& ctx, const Event& event)
    {
        if(is_active_state_def_type<states::stopped>())
        {
            process_event_in_transition
            <
                states::stopped,
                initial_state_conf,
                noop
            >(mach, ctx, event);
        }
    }

    template<class Machine, class Context, class Event>
    void stop(Machine& mach, Context& ctx, const Event& event)
    {
        if(!is_active_state_def_type<states::stopped>())
        {
            with_active_state_conf<state_conf_constant_list, stop_2>
            (
                *this,
                mach,
                ctx,
                event
            );
        }
    }

    template<class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event)
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
            if(!try_processing_event_in_transitions<candidate_transition_type_list>(mach, ctx, event))
            {
                try_processing_event_in_active_state<candidate_state_type_list>(mach, ctx, event);
            }
        }
        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            try_processing_event_in_active_state<candidate_state_type_list>(mach, ctx, event);
        }
        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
        {
            try_processing_event_in_transitions<candidate_transition_type_list>(mach, ctx, event);
        }
    }

    template<class Machine, class Context, class Event>
    void process_event(Machine& mach, Context& ctx, const Event& event, bool& processed)
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
            if(try_processing_event_in_transitions<candidate_transition_type_list>(mach, ctx, event))
            {
                processed = true;
            }
            else
            {
                try_processing_event_in_active_state<candidate_state_type_list>(mach, ctx, event, processed);
            }
        }
        else if constexpr(!must_try_processing_event_in_transitions && must_try_processing_event_in_active_state)
        {
            try_processing_event_in_active_state<candidate_state_type_list>(mach, ctx, event, processed);
        }
        else if constexpr(must_try_processing_event_in_transitions && !must_try_processing_event_in_active_state)
        {
            try_processing_event_in_transitions<candidate_transition_type_list>(mach, ctx, event, processed);
        }
    }

private:
    using root_sm_type = machine_of_t<ParentSm>;
    static constexpr auto machine_conf = root_sm_type::conf;
    using machine_conf_type = std::decay_t<decltype(machine_conf)>;

    using transition_table_type = tlu::get_t<typename ParentSm::transition_table_type_list, Index>;

    using transition_table_digest_type =
        detail::transition_table_digest<transition_table_type, region>
    ;

    using state_conf_constant_list = typename transition_table_digest_type::state_conf_constant_list;
    using state_type_list = typename transition_table_digest_type::state_type_list;
    using state_tuple_type = tlu::apply_t<state_type_list, tuple>;

    static constexpr const auto& initial_state_conf = detail::tlu::front_t<state_conf_constant_list>::value;

    struct stop_2
    {
        template<const auto& ActiveStateConf, class Machine, class Context, class Event>
        static void call(region& self, Machine& mach, Context& ctx, const Event& event)
        {
            self.process_event_in_transition
            <
                ActiveStateConf,
                states::stopped,
                noop
            >(mach, ctx, event);
        }
    };

    template<class TransitionTypeList, class Machine, class Context, class Event, class... ExtraArgs>
    bool try_processing_event_in_transitions(Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
    {
        return tlu::for_each_or
        <
            TransitionTypeList,
            try_processing_event_in_transition
        >(*this, mach, ctx, event, extra_args...);
    }

    //Check active state and guard
    struct try_processing_event_in_transition
    {
        template<class Transition, class Machine, class Context, class Event, class... ExtraArgs>
        static bool call(region& self, Machine& mach, Context& ctx, const Event& event, ExtraArgs&... extra_args)
        {
            if constexpr(is_type_pattern_v<std::decay_t<decltype(Transition::source_state_conf_pattern)>>)
            {
                //List of state defs that match with the source state pattern
                using matching_state_conf_constant_list = state_type_list_filters::by_pattern_t
                <
                    state_conf_constant_list,
                    Transition::source_state_conf_pattern
                >;

                static_assert(!tlu::empty_v<matching_state_conf_constant_list>);

                return tlu::for_each_or
                <
                    matching_state_conf_constant_list,
                    try_processing_event_in_transition_2
                    <
                        Transition::target_state_conf,
                        Transition::action,
                        Transition::guard
                    >
                >(self, mach, ctx, event, extra_args...);
            }
            else
            {
                return try_processing_event_in_transition_2
                <
                    Transition::target_state_conf,
                    Transition::action,
                    Transition::guard
                >::template call<constant<Transition::source_state_conf_pattern>>
                (
                    self,
                    mach,
                    ctx,
                    event,
                    extra_args...
                );
            }
        }
    };

    template<const auto& TargetStateConf, const auto& Action, const auto& Guard>
    struct try_processing_event_in_transition_2
    {
        template
        <
            class SourceStateConfConstant,
            class Machine,
            class Context,
            class Event,
            class... ExtraArgs
        >
        static bool call
        (
            region& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            ExtraArgs&... extra_args
        )
        {
            //Make sure the transition source state is the active state
            if(!self.is_active_state_def_type<SourceStateConfConstant::value>())
            {
                return false;
            }

            //Check guard
            if(!detail::call_action_or_guard<Guard>(mach, ctx, event))
            {
                return false;
            }

            self.process_event_in_transition
            <
                SourceStateConfConstant::value,
                TargetStateConf,
                Action
            >(mach, ctx, event, extra_args...);

            return true;
        }
    };

    template
    <
        const auto& SourceStateConf,
        const auto& TargetStateConf,
        const auto& Action,
        class Machine,
        class Context,
        class Event
    >
    void process_event_in_transition
    (
        Machine& mach,
        Context& ctx,
        const Event& event,
        bool& processed
    )
    {
        process_event_in_transition<SourceStateConf, TargetStateConf, Action>
        (
            mach,
            ctx,
            event
        );
        processed = true;
    }

    template
    <
        const auto& SourceStateConf,
        const auto& TargetStateConf,
        const auto& Action,
        class Machine,
        class Context,
        class Event
    >
    void process_event_in_transition
    (
        Machine& mach,
        Context& ctx,
        const Event& event
    )
    {
        constexpr const auto& path = region_path_of_v<region>;

        constexpr auto is_internal_transition = same_ref(TargetStateConf, null);

        if constexpr(!is_internal_transition)
        {
            if constexpr(!std::is_same_v<typename machine_conf_type::pre_state_transition_action_type, noop_ex>)
            {
                machine_conf.pre_state_transition_action_
                (
                    ctx,
                    constant_c<path>,
                    constant_c<SourceStateConf>,
                    event,
                    constant_c<TargetStateConf>
                );
            }

            if constexpr(!same_ref(SourceStateConf, states::stopped))
            {
                using source_state_t = state_traits::state_conf_to_state_t<SourceStateConf, region>;
                auto& stt = state<source_state_t>();
                stt.call_exit_action
                (
                    mach,
                    ctx,
                    event
                );
            }

            active_state_index_ = region_detail::index_of_state_conf_v
            <
                state_conf_constant_list,
                TargetStateConf
            >;
        }

        detail::call_action_or_guard<Action>
        (
            mach,
            ctx,
            event
        );

        if constexpr(!is_internal_transition)
        {
            if constexpr(!same_ref(TargetStateConf, states::stopped))
            {
                using target_state_t = state_traits::state_conf_to_state_t<TargetStateConf, region>;
                auto& stt = state<target_state_t>();
                stt.call_entry_action
                (
                    mach,
                    ctx,
                    event
                );
            }

            if constexpr(!std::is_same_v<typename machine_conf_type::post_state_transition_action_type, noop_ex>)
            {
                machine_conf.post_state_transition_action_
                (
                    ctx,
                    constant_c<path>,
                    constant_c<SourceStateConf>,
                    event,
                    constant_c<TargetStateConf>
                );
            }

            //Anonymous transition
            if constexpr(transition_table_digest_type::has_null_events)
            {
                using candidate_transition_type_list = transition_table_filters::by_event_t
                <
                    transition_table_type,
                    null_t
                >;

                try_processing_event_in_transitions<candidate_transition_type_list>(mach, ctx, null);
            }
        }
    }

    /*
    Call active_state.on_event(event)
    */
    template<class StateTypeList, class Machine, class Context, class Event, class... ExtraArgs>
    void try_processing_event_in_active_state
    (
        Machine& mach,
        Context& ctx,
        const Event& event,
        ExtraArgs&... extra_args
    )
    {
        tlu::for_each_or
        <
            StateTypeList,
            try_processing_event_in_active_state_2
        >(*this, mach, ctx, event, extra_args...);
    }

    struct try_processing_event_in_active_state_2
    {
        template<class State, class Machine, class Context, class Event, class... ExtraArgs>
        static bool call
        (
            region& self,
            Machine& mach,
            Context& ctx,
            const Event& event,
            ExtraArgs&... extra_args
        )
        {
            if(!self.is_active_state_type<State>())
            {
                return false;
            }

            if constexpr(state_traits::is_submachine_v<State>)
            {
                self.state<State>().call_internal_action
                (
                    mach,
                    ctx,
                    event,
                    extra_args...
                );
            }
            else
            {
                self.state<State>().call_internal_action
                (
                    mach,
                    ctx,
                    event
                );
                region_detail::set_to_true(extra_args...);
            }

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

    template<const auto& StateConf>
    [[nodiscard]] bool is_active_state_def_type() const
    {
        constexpr auto given_state_index = region_detail::index_of_state_conf_v
        <
            state_conf_constant_list,
            StateConf
        >;
        return given_state_index == active_state_index_;
    }

    template<class TypePattern>
    [[nodiscard]] bool does_active_state_def_match_pattern() const
    {
        auto matches = false;
        with_active_state_conf
        <
            tlu::push_back_t<state_conf_constant_list, constant<states::stopped>>,
            does_active_state_def_match_pattern_2<TypePattern>
        >(matches);
        return matches;
    }

    template<class TypePattern>
    struct does_active_state_def_match_pattern_2
    {
        template<const auto& ActiveStateConf>
        static void call([[maybe_unused]] bool& matches)
        {
            if constexpr(matches_pattern_v<constant<ActiveStateConf>, TypePattern>)
            {
                matches = true;
            }
        }
    };

    template<class StateConfConstantList, class F, class... Args>
    void with_active_state_conf(Args&&... args) const
    {
        tlu::for_each_or
        <
            StateConfConstantList,
            with_active_state_conf_2<F>
        >(*this, std::forward<Args>(args)...);
    }

    template<class F>
    struct with_active_state_conf_2
    {
        template<class StateConfConstant, class... Args>
        static bool call(const region& self, Args&&... args)
        {
            if(self.is_active_state_def_type<StateConfConstant::value>())
            {
                F::template call<StateConfConstant::value>(std::forward<Args>(args)...);
                return true;
            }
            return false;
        }
    };

    template<class State>
    auto& state()
    {
        constexpr auto state_index = tlu::index_of_v<state_type_list, State>;
        return tuple_get<state_index>(states_);
    }

    template<const auto& StateConf>
    auto& state_from_state_conf()
    {
        constexpr auto state_index = tlu::index_of_v<state_conf_constant_list, constant<StateConf>>;
        return tuple_get<state_index>(states_);
    }

    template<const auto& StateConf>
    auto& state_data_from_state_conf()
    {
        using state_t = state_traits::state_conf_to_state_t<StateConf, region>;
        return state_data<state_t>();
    }

    template<const auto& StateConf>
    const auto& state_data_from_state_conf() const
    {
        using state_t = state_traits::state_conf_to_state_t<StateConf, region>;
        return state_data<state_t>();
    }

    template<class State>
    auto& state_data()
    {
        constexpr auto state_index = tlu::index_of_v<state_type_list, State>;
        return tuple_get<state_index>(states_).data();
    }

    template<class State>
    const auto& state_data() const
    {
        constexpr auto state_index = tlu::index_of_v<state_type_list, State>;
        return tuple_get<state_index>(states_).data();
    }

    template<const auto& StateConf, class Region>
    static auto& static_state_def_data_leaf(Region& self)
    {
        using region_t = std::decay_t<Region>;
        using state_t = state_traits::state_conf_to_state_t<StateConf, region_t>;
        return self.template state_data<state_t>();
    }

    template<const auto& StateRegionPath, const auto& StateConf, class Region>
    static auto& static_state_def_data(Region& self)
    {
        using state_region_path_t = std::decay_t<decltype(StateRegionPath)>;

        if constexpr(tlu::size_v<state_region_path_t> == 0)
        {
            return static_state_def_data_leaf<StateConf>(self);
        }
        else
        {
            constexpr const auto& submach_conf = tlu::front_t<state_region_path_t>::machine_conf;
            constexpr auto submachine_index = tlu::index_of_v<typename Region::state_conf_constant_list, constant<submach_conf>>;
            auto& submach = tuple_get<submachine_index>(self.states_);
            return submach.template state_def_data<StateRegionPath, StateConf>(); //recursive
        }
    }

    state_tuple_type states_;
    int active_state_index_ = region_detail::index_of_state_conf_v<state_conf_constant_list, states::stopped>;
};

} //namespace

#endif

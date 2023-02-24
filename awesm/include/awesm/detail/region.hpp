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
#include "../sm_conf.hpp"
#include "../states.hpp"
#include "../null.hpp"
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
}

template<class ParentSm, int Index, auto TransitionTableFn>
class region;

template<class ParentSm, int Index, auto TransitionTableFn>
struct region_path_of<region<ParentSm, Index, TransitionTableFn>>
{
    using type = typename region_path_of_t<ParentSm>::template add<ParentSm, Index>;
};

template<class ParentSm, int Index, auto TransitionTableFn>
struct root_conf_of<region<ParentSm, Index, TransitionTableFn>>
{
    using type = root_conf_of_t<ParentSm>;
};

template<class ParentSm, int Index, auto TransitionTableFn>
struct root_sm_of<region<ParentSm, Index, TransitionTableFn>>
{
    using type = root_sm_of_t<ParentSm>;
};

template<class ParentSm, int Index, auto TransitionTableFn>
class region
{
    public:
        using context_type = typename ParentSm::context_type;
        using conf = root_conf_of_t<region>;

        region(ParentSm& parent_sm, context_type& ctx):
            parent_sm_(parent_sm),
            ctx_(ctx),
            state_holders_(parent_sm.get_root_sm(), ctx)
        {
        }

        region(const region&) = delete;
        region(region&&) = delete;
        region& operator=(const region&) = delete;
        region& operator=(region&&) = delete;
        ~region() = default;

        template<class StateRelativeRegionPath, class State>
        [[nodiscard]] bool is_active_state() const
        {
            if constexpr(tlu::size_v<StateRelativeRegionPath> == 0)
            {
                return is_active_state<State>();
            }
            else
            {
                using subsm_t = typename tlu::front_t<StateRelativeRegionPath>::sm_type;
                const auto& state = get_state<subsm_t>();
                return state.template is_active_state<StateRelativeRegionPath, State>();
            }
        }

        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            if constexpr(is_type_pattern_v<State>)
            {
                return is_active_state_pattern<State>();
            }
            else
            {
                return is_active_state_type<State>();
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
                with_active_state<stop_2>
                (
                    *this,
                    event
                );
            }
        }

        template<class Event>
        void process_event(const Event& event)
        {
            if(!process_event_in_transition_table(event))
            {
                process_event_in_active_state(event);
            }
        }

    private:
        using option_mix_type = typename conf::option_mix_type;

        using transition_table_type = decltype(TransitionTableFn());

        using transition_table_digest_type =
            detail::transition_table_digest<transition_table_type, region, context_type>
        ;
        using state_tuple_type = typename transition_table_digest_type::state_tuple_type;
        using wrapped_state_holder_tuple_type =
            typename transition_table_digest_type::wrapped_state_holder_tuple_type
        ;

        using initial_state_type = detail::tlu::front_t<state_tuple_type>;

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

        template<class Event>
        bool process_event_in_transition_table(const Event& event)
        {
            using filtered_transition_table_t = transition_table_filters::by_event_t
            <
                transition_table_type,
                Event
            >;

            if constexpr(tlu::empty_v<filtered_transition_table_t>)
            {
                return false;
            }
            else
            {
                return tlu::for_each_or
                <
                    filtered_transition_table_t,
                    try_processing_event_in_transition
                >(*this, event);
            }
        }

        //Check active state and guard
        struct try_processing_event_in_transition
        {
            template<class Transition, class Event>
            static bool call(region& self, const Event& event)
            {
                using source_state_t = typename Transition::source_state_type_pattern;
                using target_state_t = typename Transition::target_state_type;

                if constexpr(is_type_pattern_v<source_state_t>)
                {
                    using matching_state_type_list = state_type_list_filters::by_pattern_t
                    <
                        state_tuple_type,
                        source_state_t
                    >;

                    static_assert(!tlu::empty_v<matching_state_type_list>);

                    return tlu::for_each_or
                    <
                        matching_state_type_list,
                        try_processing_event_in_transition_2
                        <
                            target_state_t,
                            Transition::get_action(),
                            Transition::get_guard()
                        >
                    >(self, event);
                }
                else
                {
                    return try_processing_event_in_transition_2
                    <
                        target_state_t,
                        Transition::get_action(),
                        Transition::get_guard()
                    >::template call<source_state_t>(self, event);
                }
            }
        };

        template
        <
            class TargetState,
            const auto& Action,
            const auto& Guard
        >
        struct try_processing_event_in_transition_2
        {
            template<class SourceState, class Event>
            static bool call(region& self, const Event& event)
            {
                //Make sure the transition source state is the active state
                if(!self.is_active_state_type<SourceState>())
                {
                    return false;
                }

                //Check guard
                if(!detail::call_action_or_guard(Guard, &self.get_root_sm(), self.ctx_, &event))
                {
                    return false;
                }

                self.process_event_in_transition
                <
                    SourceState,
                    TargetState,
                    Action
                >(event);

                return true;
            }
        };

        template<class SourceState, class TargetState, const auto& Action, class Event>
        void process_event_in_transition([[maybe_unused]] const Event& event)
        {
            using path_t = region_path_of_t<region>;

            constexpr auto is_internal_transition =
                std::is_same_v<TargetState, null>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains_v<option_mix_type, sm_opts::before_state_transition>)
                {
                    get_root_sm().get_def().template before_state_transition
                    <
                        path_t,
                        SourceState,
                        Event,
                        TargetState
                    >(event);
                }

                if constexpr(!std::is_same_v<SourceState, states::stopped>)
                {
                    detail::call_on_exit
                    (
                        get_state<SourceState>(),
                        get_root_sm(),
                        event
                    );
                }

                active_state_index_ = index_of_state_v
                <
                    state_tuple_type,
                    TargetState
                >;
            }

            detail::call_action_or_guard
            (
                Action,
                &get_root_sm(),
                ctx_,
                &event
            );

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains_v<option_mix_type, sm_opts::before_entry>)
                {
                    get_root_sm().get_def().template before_entry
                    <
                        path_t,
                        SourceState,
                        Event,
                        TargetState
                    >(event);
                }

                if constexpr(!std::is_same_v<TargetState, states::stopped>)
                {
                    detail::call_on_entry
                    (
                        get_state<TargetState>(),
                        get_root_sm(),
                        event
                    );
                }

                if constexpr(tlu::contains_v<option_mix_type, sm_opts::after_state_transition>)
                {
                    get_root_sm().get_def().template after_state_transition
                    <
                        path_t,
                        SourceState,
                        Event,
                        TargetState
                    >(event);
                }

                //Anonymous transition
                if constexpr(transition_table_digest_type::has_null_events)
                {
                    process_event_in_transition_table(null{});
                }
            }
        }

        /*
        Call active_state.on_event(event)
        */
        template<class Event>
        void process_event_in_active_state(const Event& event)
        {
            using filtered_state_type_list =
                state_type_list_filters::by_required_on_event_t
                <
                    state_tuple_type,
                    region,
                    context_type,
                    Event
                >
            ;

            if constexpr(!tlu::empty_v<filtered_state_type_list>)
            {
                tlu::for_each_or
                <
                    filtered_state_type_list,
                    process_event_in_active_state_2
                >(*this, event);
            }
        }

        struct process_event_in_active_state_2
        {
            template<class State, class Event>
            static bool call(region& self, const Event& event)
            {
                if(!self.is_active_state_type<State>())
                {
                    return false;
                }

                auto& state = self.get_state<State>();
                call_on_event(state, event);

                return false;
            }
        };

        template<class State>
        [[nodiscard]] bool is_active_state_type() const
        {
            constexpr auto given_state_index = index_of_state_v
            <
                state_tuple_type,
                State
            >;
            return given_state_index == active_state_index_;
        }

        template<class StatePattern>
        [[nodiscard]] bool is_active_state_pattern() const
        {
            auto active_state_matches_pattern = false;
            with_active_state_or_stopped<is_active_state_pattern_2<StatePattern>>(active_state_matches_pattern);
            return active_state_matches_pattern;
        }

        template<class StatePattern>
        struct is_active_state_pattern_2
        {
            template<class ActiveState>
            static void call([[maybe_unused]] bool& active_state_matches_pattern)
            {
                if constexpr(matches_pattern_v<ActiveState, StatePattern>)
                {
                    active_state_matches_pattern = true;
                }
            }
        };

        template<class F, class... Args>
        void with_active_state(Args&&... args) const
        {
            tlu::for_each_or
            <
                state_tuple_type,
                with_active_state_2<F>
            >(*this, std::forward<Args>(args)...);
        }

        template<class F, class... Args>
        void with_active_state_or_stopped(Args&&... args) const
        {
            tlu::for_each_or
            <
                tlu::push_back_t<state_tuple_type, states::stopped>,
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

        auto& get_root_sm()
        {
            return parent_sm_.get_root_sm();
        }

        template<class State>
        auto& get_state()
        {
            using wrapped_state_t = state_traits::wrap_t<State, region, context_type>;
            return get<sm_object_holder<wrapped_state_t>>(state_holders_).get();
        }

        template<class State>
        const auto& get_state() const
        {
            using wrapped_state_t = state_traits::wrap_t<State, region, context_type>;
            return get<sm_object_holder<wrapped_state_t>>(state_holders_).get();
        }

        ParentSm& parent_sm_;
        context_type& ctx_;
        wrapped_state_holder_tuple_type state_holders_;

        int active_state_index_ = index_of_state_v<state_tuple_type, states::stopped>;
};

} //namespace

#endif

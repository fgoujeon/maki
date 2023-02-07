//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "../sm_conf.hpp"
#include "../states.hpp"
#include "../null.hpp"
#include "state_traits.hpp"
#include "sm_path.hpp"
#include "call_member.hpp"
#include "resolve_transition_table.hpp"
#include "transition_table_digest.hpp"
#include "tlu.hpp"
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

template<class RegionPath, auto TransitionTableFn>
class region
{
    public:
        using sm_type = region_path_to_sm_t<RegionPath>;
        using context_type = typename sm_type::context_type;
        using conf = typename sm_type::conf;

        explicit region(sm_type& mach):
            mach_(mach),
            state_holders_(mach, mach.get_context())
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
            using fake_row = row<states::stopped, Event, initial_state_type>;
            try_processing_event_in_row<fake_row>(event);
        }

        template<class Event>
        void stop(const Event& event)
        {
            detail::tlu::apply_t<state_tuple_type, stop_helper>::call(*this, event);
        }

        template<class Event>
        void process_event(const Event& event)
        {
            process_event_in_active_state(event);
            process_event_in_transition_table(event);
        }

    private:
        using option_mix_type = typename conf::option_mix_type;

        using unresolved_transition_table_type = decltype(TransitionTableFn());

        using transition_table_digest_type =
            detail::transition_table_digest<RegionPath, unresolved_transition_table_type>
        ;
        using state_tuple_type = typename transition_table_digest_type::state_tuple_type;
        using wrapped_state_holder_tuple_type =
            typename transition_table_digest_type::wrapped_state_holder_tuple_type
        ;

        using initial_state_type = detail::tlu::front_t<state_tuple_type>;

        using transition_table_type = detail::resolve_transition_table_t
        <
            unresolved_transition_table_type,
            state_tuple_type,
            transition_table_digest_type::has_source_state_patterns
        >;

        template<class... States>
        struct stop_helper
        {
            template<class Event>
            static void call(region& self, const Event& event)
            {
                (self.stop_2<States>(event) || ...);
            }
        };

        template<class State, class Event>
        bool stop_2([[maybe_unused]] const Event& event)
        {
            if constexpr(std::is_same_v<State, states::stopped>)
            {
                return false;
            }
            else
            {
                using fake_row = row<State, Event, states::stopped>;
                return try_processing_event_in_row<fake_row>(event);
            }
        }

        template<class Event>
        void process_event_in_transition_table(const Event& event)
        {
            detail::tlu::apply_t
            <
                transition_table_type,
                process_event_in_transition_table_helper
            >::process(*this, event);
        }

        template<class Row, class... Rows>
        struct process_event_in_transition_table_helper
        {
            template<class Event>
            static void process
            (
                [[maybe_unused]] region& self,
                [[maybe_unused]] const Event& event
            )
            {
                if constexpr(std::is_same_v<Event, typename Row::event_type>)
                {
                    if(self.try_processing_event_in_row<Row>(event))
                    {
                        return;
                    }
                }

                if constexpr(sizeof...(Rows) != 0)
                {
                    process_event_in_transition_table_helper<Rows...>::process(self, event);
                }
            }
        };

        //Check active state and guard
        template<class Row, class Event>
        bool try_processing_event_in_row(const Event& event)
        {
            using source_state_type = typename Row::source_state_type;

            //Make sure the transition source state is the active state
            if(!is_active_state_type<source_state_type>())
            {
                return false;
            }

            //Check guard
            if(!detail::call_action_or_guard(Row::get_guard(), mach_, &event))
            {
                return false;
            }

            process_event_in_row<Row>(event);

            return true;
        }

        template<class Row, class Event>
        void process_event_in_row([[maybe_unused]] const Event& event)
        {
            using source_state_type = typename Row::source_state_type;
            using target_state_type = typename Row::target_state_type;

            constexpr auto is_internal_transition =
                std::is_same_v<target_state_type, null>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains_v<option_mix_type, sm_opts::before_state_transition>)
                {
                    mach_.get_def().template before_state_transition
                    <
                        RegionPath,
                        source_state_type,
                        Event,
                        target_state_type
                    >(event);
                }

                if constexpr(!std::is_same_v<source_state_type, states::stopped>)
                {
                    detail::call_on_exit
                    (
                        get_state<source_state_type>(),
                        event
                    );
                }

                active_state_index_ = index_of_state_v
                <
                    state_tuple_type,
                    target_state_type
                >;
            }

            detail::call_action_or_guard
            (
                Row::get_action(),
                mach_,
                &event
            );

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains_v<option_mix_type, sm_opts::before_entry>)
                {
                    mach_.get_def().template before_entry
                    <
                        RegionPath,
                        source_state_type,
                        Event,
                        target_state_type
                    >(event);
                }

                if constexpr(!std::is_same_v<target_state_type, states::stopped>)
                {
                    detail::call_on_entry
                    (
                        get_state<target_state_type>(),
                        event
                    );
                }

                if constexpr(tlu::contains_v<option_mix_type, sm_opts::after_state_transition>)
                {
                    mach_.get_def().template after_state_transition
                    <
                        RegionPath,
                        source_state_type,
                        Event,
                        target_state_type
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
            detail::tlu::apply_t
            <
                state_tuple_type,
                process_event_in_active_state_helper
            >::process(*this, event);
        }

        template<class State, class... States>
        struct process_event_in_active_state_helper
        {
            template<class Event>
            static void process
            (
                [[maybe_unused]] region& self,
                [[maybe_unused]] const Event& event
            )
            {
                using wrapped_state_t = state_traits::wrap_t<State, RegionPath>;
                if constexpr(state_traits::requires_on_event_v<wrapped_state_t, Event>)
                {
                    auto& state = self.get_state<State>();
                    if(self.is_active_state_type<State>())
                    {
                        call_on_event(state, event);
                        return;
                    }
                }

                if constexpr(sizeof...(States) != 0)
                {
                    process_event_in_active_state_helper<States...>::process(self, event);
                }
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
            return tlu::apply_t
            <
                tlu::push_back_t<state_tuple_type, states::stopped>,
                is_active_state_pattern_helper
            >::template call<StatePattern>(*this);
        }

        template<class... States>
        struct is_active_state_pattern_helper
        {
            template<class StatePattern>
            static bool call(const region& self)
            {
                return
                (
                    self.is_active_state_pattern_2<StatePattern, States>() ||
                    ...
                );
            }
        };

        template<class StatePattern, class State>
        [[nodiscard]] bool is_active_state_pattern_2() const
        {
            if constexpr(type_pattern_matches<StatePattern, State>())
            {
                return is_active_state_type<State>();
            }
            else
            {
                return false;
            }
        }

        template<class State>
        auto& get_state()
        {
            using wrapped_state_t = state_traits::wrap_t<State, RegionPath>;
            return get<sm_object_holder<wrapped_state_t>>(state_holders_).get();
        }

        template<class State>
        const auto& get_state() const
        {
            using wrapped_state_t = state_traits::wrap_t<State, RegionPath>;
            return get<sm_object_holder<wrapped_state_t>>(state_holders_).get();
        }

        sm_type& mach_;
        wrapped_state_holder_tuple_type state_holders_;

        int active_state_index_ = index_of_state_v<state_tuple_type, states::stopped>;
};

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "../sm_conf.hpp"
#include "../null.hpp"
#include "state_traits.hpp"
#include "sm_path.hpp"
#include "null_state.hpp"
#include "call_member.hpp"
#include "resolve_transition_table.hpp"
#include "transition_table_digest.hpp"
#include "ignore_unused.hpp"
#include "tlu/apply.hpp"
#include <type_traits>
#include <exception>

namespace awesm::detail
{

template<class RegionPath, class TransitionTable>
class region
{
    public:
        using sm_type = region_path_to_sm_t<RegionPath>;
        using context_type = typename sm_type::context_type;
        using conf_type = typename sm_type::conf_type;

        explicit region(sm_type& mach):
            mach_(mach),
            states_(mach, mach.get_context())
        {
        }

        region(const region&) = delete;
        region(region&&) = delete;
        region& operator=(const region&) = delete;
        region& operator=(region&&) = delete;
        ~region() = default;

        //Check whether the given State is the active state type
        template<class State>
        [[nodiscard]] bool is_active_state() const
        {
            constexpr auto given_state_index = detail::tlu::index_of_v
            <
                state_tuple_type,
                State
            >;
            return active_state_index_ == given_state_index;
        }

        template<class State>
        const auto& get_state() const
        {
            return get<state_wrapper_t<RegionPath, State>>(states_);
        }

        template<class Event>
        void start(const Event& event)
        {
            using fake_row = row<null_state, Event, initial_state_type>;
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
        using unresolved_transition_table_type = TransitionTable;

        using transition_table_digest_type =
            detail::transition_table_digest<RegionPath, unresolved_transition_table_type>
        ;
        using state_tuple_type = typename transition_table_digest_type::state_tuple_type;
        using wrapped_state_holder_tuple_type =
            typename transition_table_digest_type::wrapped_state_holder_tuple_type
        ;

        using initial_state_type = detail::tlu::at_t<state_tuple_type, 1>; //0 being null_state

        using transition_table_type = detail::resolve_transition_table_t
        <
            unresolved_transition_table_type,
            state_tuple_type,
            transition_table_digest_type::has_source_state_patterns
        >;

        //Used to call client code
        template<class F, class... Args>
        void safe_call(F&& callback, Args&&... args)
        {
            try
            {
                callback(std::forward<Args>(args)...);
            }
            catch(...)
            {
                mach_.process_exception(std::current_exception());
            }
        }

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
        bool stop_2(const Event& event)
        {
            using fake_row = row<State, Event, null_state>;
            return try_processing_event_in_row<fake_row>(event);
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
            static void process(region& self, const Event& event)
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

                ignore_unused(self, event);
            }
        };

        //Check active state and guard
        template<class Row, class Event>
        bool try_processing_event_in_row(const Event& event)
        {
            using source_state_type = typename Row::source_state_type;

            //Make sure the transition source state is the active state
            if(!is_active_state<source_state_type>())
            {
                return false;
            }

            auto processed = false;
            safe_call
            (
                [](region& self, const Event& event, bool& processed)
                {
                    if
                    (
                        !detail::call_action_or_guard
                        (
                            Row::get_guard(),
                            self.mach_,
                            &event
                        )
                    )
                    {
                        return;
                    }

                    processed = true;
                    self.process_event_in_row<Row>(event);
                },
                *this,
                event,
                processed
            );
            return processed;
        }

        template<class Row, class Event>
        void process_event_in_row(const Event& event)
        {
            using source_state_type = typename Row::source_state_type;
            using target_state_type = typename Row::target_state_type;

            ignore_unused(event);

            constexpr auto is_internal_transition =
                std::is_same_v<target_state_type, null>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains_v<conf_type, sm_options::before_state_transition>)
                {
                    mach_.def_.template before_state_transition
                    <
                        RegionPath,
                        source_state_type,
                        Event,
                        target_state_type
                    >(event);
                }

                if constexpr(state_traits::requires_on_exit_v<state_wrapper_t<RegionPath, source_state_type>>)
                {
                    detail::call_on_exit
                    (
                        get<state_wrapper_t<RegionPath, source_state_type>>(states_),
                        &event
                    );
                }

                active_state_index_ = detail::tlu::index_of_v
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
                if constexpr(tlu::contains_v<conf_type, sm_options::before_entry>)
                {
                    mach_.def_.template before_entry
                    <
                        RegionPath,
                        source_state_type,
                        Event,
                        target_state_type
                    >(event);
                }

                if constexpr
                (
                    !is_internal_transition && //for VS2017
                    state_traits::requires_on_entry_v<state_wrapper_t<RegionPath, target_state_type>>
                )
                {
                    detail::call_on_entry
                    (
                        get<state_wrapper_t<RegionPath, target_state_type>>(states_),
                        &event
                    );
                }

                if constexpr(tlu::contains_v<conf_type, sm_options::after_state_transition>)
                {
                    mach_.def_.template after_state_transition
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
            static void process(region& self, const Event& event)
            {
                using wrapped_state_t = state_wrapper_t<RegionPath, State>;
                if constexpr(state_traits::requires_on_event_v<wrapped_state_t, Event>)
                {
                    auto& state = get<wrapped_state_t>(self.states_);
                    if(self.is_active_state<State>())
                    {
                        self.safe_call
                        (
                            [](wrapped_state_t& state, const Event& event)
                            {
                                call_on_event(state, event);
                            },
                            state,
                            event
                        );
                        return;
                    }
                }

                if constexpr(sizeof...(States) != 0)
                {
                    process_event_in_active_state_helper<States...>::process(self, event);
                }

                ignore_unused(self, event);
            }
        };

        sm_type& mach_;
        wrapped_state_holder_tuple_type states_;

        int active_state_index_ = 0;
};

} //namespace

#endif

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
#include "transition_table_filters.hpp"
#include "alternative_lazy.hpp"
#include "any_container.hpp"
#include "ignore_unused.hpp"
#include "event_processing_type.hpp"
#include "tlu/apply.hpp"
#include <type_traits>

namespace awesm::detail
{

template<template<int> class F, class IntegerSequence>
struct make_jump_table;

template<template<int> class F, int... Values>
struct make_jump_table<F, std::integer_sequence<int, Values...>>
{
    template<class Arg0, class Arg1>
    static void call(const int value, Arg0& arg0, Arg1& arg1)
    {
        ignore_unused
        (
            (value == Values ? (F<Values>::call(arg0, arg1), 0) : 0)...
        );
    }
};

template<class RegionPath, class TransitionTable>
class region
{
    public:
        using sm_t = region_path_to_sm_t<RegionPath>;
        using context_t = typename sm_t::context_t;
        using conf_t = typename sm_t::conf_t;

        explicit region(sm_t& mach, context_t& ctx):
            mach_(mach),
            ctx_(ctx),
            states_(mach, ctx)
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
            constexpr auto given_state_index = detail::tlu::get_index
            <
                state_tuple_t,
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
            if(is_active_state<null_state>())
            {
                safe_call
                (
                    [&]
                    {
                        using fake_row = row<null_state, Event, initial_state_t>;
                        process_event_in_row<fake_row>(event);
                    }
                );
            }
        }

        template<class Event>
        void stop(const Event& event)
        {
            safe_call
            (
                [&]
                {
                    detail::tlu::apply<state_tuple_t, stop_helper>::call(*this, event);
                }
            );
        }

        template<class Event>
        void process_event(const Event& event)
        {
            process_event_in_active_state(event);
            process_event_in_transition_table(event);
        }

    private:
        using unresolved_transition_table_t = TransitionTable;

        using transition_table_digest_t =
            detail::transition_table_digest<RegionPath, unresolved_transition_table_t>
        ;
        using state_tuple_t = typename transition_table_digest_t::state_tuple;
        using wrapped_state_holder_tuple_t =
            typename transition_table_digest_t::wrapped_state_holder_tuple
        ;

        using initial_state_t = detail::tlu::at<state_tuple_t, 1>; //0 being null_state

        using transition_table_t = detail::resolve_transition_table_t
        <
            unresolved_transition_table_t,
            state_tuple_t,
            transition_table_digest_t::has_source_state_patterns
        >;

        //Used to call client code
        template<class F>
        void safe_call(F&& callback)
        {
            try
            {
                callback();
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
            if(is_active_state<State>())
            {
                using fake_row = row<State, Event, null_state>;
                process_event_in_row<fake_row>(event);
                return true;
            }
            return false;
        }

        template<class Event>
        void process_event_in_transition_table(const Event& event)
        {
            make_jump_table
            <
                process_event_in_transition_table_2,
                std::make_integer_sequence<int, tlu::size_v<state_tuple_t>>
            >::call(active_state_index_, *this, event);
        }

        template<int StateIndex>
        struct process_event_in_transition_table_2
        {
            template<class Event>
            static void call(region& self, const Event& event)
            {
                using state_t = tlu::at<state_tuple_t, StateIndex>;
                for_state<state_t>::process_event_in_transition_table(self, event);
            }
        };

        template<class... Rows>
        struct for_state_helper
        {
            template<class TransitionTable2, class Event>
            static void process(region& self, const Event& event)
            {
                using filtered_transition_table_t = filter_transition_table_by_event
                <
                    TransitionTable2,
                    Event
                >;

                detail::tlu::apply
                <
                    filtered_transition_table_t,
                    process_event_in_transition_table_helper
                >::process(self, event);
            }
        };

        template<class State>
        struct for_state
        {
            using state_transition_table_t = filter_transition_table_by_source_state
            <
                transition_table_t,
                State
            >;

            using helper = detail::tlu::apply
            <
                state_transition_table_t,
                for_state_helper
            >;

            template<class Event>
            static void process_event_in_transition_table(region& self, const Event& event)
            {
                self.safe_call
                (
                    [&]
                    {
                        helper::template process<state_transition_table_t>(self, event);
                    }
                );
            }
        };

        template<class... Rows>
        struct process_event_in_transition_table_helper
        {
            template<class Event>
            static void process(region& self, const Event& event)
            {
                ignore_unused((self.try_processing_event_in_row<Rows>(event) || ...));
            }
        };

        //Check guard
        template<class Row, class Event>
        bool try_processing_event_in_row(const Event& event)
        {
            if
            (
                detail::call_action_or_guard
                (
                    Row::get_guard(),
                    &mach_,
                    &ctx_,
                    &event
                )
            )
            {
                process_event_in_row<Row>(event);
                return true;
            }

            return false;
        }

        template<class Row, class Event>
        void process_event_in_row(const Event& event)
        {
            using source_state_t = typename Row::source_state_t;
            using target_state_t = typename Row::target_state_t;

            ignore_unused(event);

            constexpr auto is_internal_transition =
                std::is_same_v<target_state_t, null>
            ;

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains<conf_t, sm_options::before_state_transition>)
                {
                    mach_.def_.template before_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr(state_traits::requires_on_exit_v<state_wrapper_t<RegionPath, source_state_t>>)
                {
                    detail::call_on_exit
                    (
                        get<state_wrapper_t<RegionPath, source_state_t>>(states_),
                        &event
                    );
                }

                active_state_index_ = detail::tlu::get_index
                <
                    state_tuple_t,
                    target_state_t
                >;
            }

            detail::call_action_or_guard
            (
                Row::get_action(),
                &mach_,
                &ctx_,
                &event
            );

            if constexpr(!is_internal_transition)
            {
                if constexpr(tlu::contains<conf_t, sm_options::before_entry>)
                {
                    mach_.def_.template before_entry
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                if constexpr
                (
                    !is_internal_transition && //for VS2017
                    state_traits::requires_on_entry_v<state_wrapper_t<RegionPath, target_state_t>>
                )
                {
                    detail::call_on_entry
                    (
                        get<state_wrapper_t<RegionPath, target_state_t>>(states_),
                        &event
                    );
                }

                if constexpr(tlu::contains<conf_t, sm_options::after_state_transition>)
                {
                    mach_.def_.template after_state_transition
                    <
                        RegionPath,
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }

                //Anonymous transition
                if constexpr(transition_table_digest_t::has_comp_events)
                {
                    process_event_in_transition_table(events::comp{});
                }
            }
        }

        /*
        Call active_state.on_event(event)
        */
        template<class Event>
        void process_event_in_active_state(const Event& event)
        {
            detail::tlu::apply
            <
                state_tuple_t,
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
                            [&]
                            {
                                call_on_event(state, event);
                            }
                        );
                        return;
                    }
                }

                if constexpr(sizeof...(States) != 0)
                {
                    process_event_in_active_state_helper<States...>::process(self, event);
                }
            }
        };

        sm_t& mach_;
        context_t& ctx_;
        wrapped_state_holder_tuple_t states_;

        int active_state_index_ = 0;
};

} //namespace

#endif

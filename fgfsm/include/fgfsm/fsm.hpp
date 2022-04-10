//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_HPP
#define FGFSM_FSM_HPP

#include "fsm_configuration.hpp"
#include "internal_transition_policy.hpp"
#include "state_transition_policy.hpp"
#include "any.hpp"
#include "none.hpp"
#include "any_copy.hpp"
#include "detail/for_each.hpp"
#include "detail/make_tuple.hpp"
#include "detail/resolve_transition_table.hpp"
#include "detail/transition_table_digest.hpp"
#include "detail/ignore_unused.hpp"
#include <queue>
#include <type_traits>

namespace fgfsm
{

template<class TransitionTable, class Configuration = fsm_configuration>
class fsm
{
    private:
        static_assert
        (
            std::is_base_of_v<fsm_configuration, Configuration>,
            "Given configuration type must inherit from fgfsm::fsm_configuration."
        );

        using pre_transition_event_handler = typename Configuration::pre_transition_event_handler;
        using internal_transition_policy = typename Configuration::internal_transition_policy;
        using state_transition_policy = typename Configuration::state_transition_policy;

        using transition_table_digest =
            detail::transition_table_digest<TransitionTable>
        ;
        using state_tuple  = typename transition_table_digest::state_tuple;
        using action_tuple = typename transition_table_digest::action_tuple;
        using guard_tuple  = typename transition_table_digest::guard_tuple;

        /*
        Calling detail::resolve_transition_table<> isn't free. We need this
        trick to avoid the call when it's unnecessary (i.e. when there's no
        any-start-state in the transition table).
        */
        static constexpr auto make_transition_table_ptr()
        {
            if constexpr(transition_table_digest::has_any_start_states)
            {
                using ptr_t = detail::resolve_transition_table
                <
                    TransitionTable,
                    state_tuple
                >*;
                return ptr_t{};
            }
            else
            {
                using ptr_t = TransitionTable*;
                return ptr_t{};
            }
        }
        using transition_table = std::remove_pointer_t
        <
            decltype(make_transition_table_ptr())
        >;

    public:
        template<class Context>
        fsm(Context& context):
            states_(detail::make_tuple<state_tuple>(context)),
            actions_(detail::make_tuple<action_tuple>(context)),
            guards_(detail::make_tuple<guard_tuple>(context)),
            pre_transition_event_handler_{context},
            internal_transition_policy_{context},
            state_transition_policy_{context}
        {
        }

        //Check whether the given State is the active state type
        template<class State>
        bool is_active_state() const
        {
            constexpr auto given_state_index = detail::tlu::get_index
            <
                state_tuple,
                State
            >;
            return given_state_index == active_state_index_;
        }

        void process_event(const any_cref& event)
        {
            if constexpr(Configuration::enable_run_to_completion)
            {
                //Defer event processing in case of recursive call
                if(processing_event_)
                {
                    deferred_events_.push(any_copy{event});
                    return;
                }

                struct processing_event_guard
                {
                    processing_event_guard(bool& b):
                        b(b)
                    {
                        b = true;
                    }

                    ~processing_event_guard()
                    {
                        b = false;
                    }

                    bool& b;
                };
                auto processing_guard = processing_event_guard{processing_event_};

                process_event_once(event);

                //Process deferred event processings
                while(!deferred_events_.empty())
                {
                    process_event_once(deferred_events_.front());
                    deferred_events_.pop();
                }
            }
            else
            {
                process_event_once(event);
            }
        }

    private:
        template<class F>
        void visit_active_state(F&& f)
        {
            detail::for_each
            (
                states_,
                [this, &f](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    if(is_active_state<state>())
                        f(s);
                }
            );
        }

        void process_event_once(const any_cref& event)
        {
            pre_transition_event_handler_.on_event(event);
            if constexpr(Configuration::enable_in_state_internal_transitions)
                process_event_in_active_state(event);
            process_event_in_transition_table(event);
        }

        /*
        Try and trigger a transition and potential subsequent anonymous
        transitions, if any.
        */
        void process_event_in_transition_table(const any_cref& event)
        {
            const bool processed = process_event_in_transition_table_once(event);
            detail::ignore_unused(processed);

            //Anonymous transitions
            if constexpr(transition_table_digest::has_none_events)
                if(processed)
                    while(process_event_in_transition_table_once(none{}));
        }

        //Try and trigger one transition
        bool process_event_in_transition_table_once(const any_cref& event)
        {
            bool processed = false;

            const auto helper = process_event_in_transition_table_once_helper
            {
                *this,
                event,
                processed
            };

            //For each row
            detail::tlu::for_each<transition_table>(helper);

            return processed;
        }

        struct process_event_in_transition_table_once_helper
        {
            template<class Transition>
            void operator()() const
            {
                using transition_start_state  = typename Transition::start_state;
                using transition_event        = typename Transition::event;
                using transition_target_state = typename Transition::target_state;
                using transition_action       = typename Transition::action;
                using transition_guard        = typename Transition::guard;

                //Make sure we don't trigger more than one transition
                if(processed)
                    return;

                //Make sure the transition start state is the active state
                if(!self.is_active_state<transition_start_state>())
                    return;

                //Make sure the event type is the one described the transition
                if(!event.is<transition_event>())
                    return;

                auto& start_state =
                    std::get<transition_start_state>(self.states_)
                ;

                const auto ptarget_state = [&]() -> transition_target_state*
                {
                    if constexpr(std::is_same_v<transition_target_state, none>)
                        return nullptr;
                    else
                        return &std::get<transition_target_state>(self.states_);
                }();

                const auto pguard = [&]() -> transition_guard*
                {
                    if constexpr(std::is_same_v<transition_guard, none>)
                        return nullptr;
                    else
                        return &std::get<transition_guard>(self.guards_);
                }();

                const auto paction = [&]() -> transition_action*
                {
                    if constexpr(std::is_same_v<transition_action, none>)
                        return nullptr;
                    else
                        return &std::get<transition_action>(self.actions_);
                }();

                const auto target_state_index = [&]
                {
                    if constexpr(std::is_same_v<transition_target_state, none>)
                        return -1; //whatever, ignored in none case
                    else
                        return detail::tlu::get_index
                        <
                            state_tuple,
                            transition_target_state
                        >;
                }();

                auto helper = state_transition_policy_helper
                <
                    transition_start_state,
                    transition_event,
                    transition_target_state,
                    transition_action,
                    transition_guard
                >
                {
                    start_state,
                    *event.get_if<transition_event>(),
                    ptarget_state,
                    paction,
                    pguard,
                    self.active_state_index_,
                    processed,
                    target_state_index
                };

                //Perform the transition
                self.state_transition_policy_.do_transition(helper);
            }

            fsm& self;
            const any_cref& event;
            bool& processed;
        };

        /*
        Call active_state.on_event(event)
        */
        void process_event_in_active_state(const any_cref& event)
        {
            visit_active_state
            (
                [this, &event](auto& s)
                {
                    using state = std::decay_t<decltype(s)>;
                    auto helper = internal_transition_policy_helper
                    <
                        state
                    >{s, event};
                    internal_transition_policy_.do_transition(helper);
                }
            );
        }

    private:
        state_tuple states_;
        action_tuple actions_;
        guard_tuple guards_;
        pre_transition_event_handler pre_transition_event_handler_;
        internal_transition_policy internal_transition_policy_;
        state_transition_policy state_transition_policy_;

        int active_state_index_ = 0;
        bool processing_event_ = false;
        std::queue<any_copy> deferred_events_;
};

} //namespace

#endif

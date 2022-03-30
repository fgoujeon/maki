//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_HPP
#define FGFSM_FSM_HPP

#include "fsm_configuration.hpp"
#include "state_transition_policy.hpp"
#include "internal_transition_policy.hpp"
#include "any.hpp"
#include "none.hpp"
#include "any_copy.hpp"
#include "detail/for_each.hpp"
#include "detail/make_tuple.hpp"
#include "detail/transition_table_digest.hpp"
#include "detail/ignore_unused.hpp"
#include <queue>
#include <type_traits>

namespace fgfsm
{

template<class Configuration>
class fsm
{
    private:
        static_assert
        (
            std::is_base_of_v<fsm_configuration, Configuration>,
            "Given configuration type must inherit from fgfsm::fsm_configuration."
        );

        using context = typename Configuration::context;
        using transition_table = typename Configuration::transition_table;

        using state_transition_policy = typename Configuration::state_transition_policy;
        using internal_transition_policy = typename Configuration::internal_transition_policy;

        using transition_table_digest =
            detail::transition_table_digest<transition_table>
        ;
        using state_tuple  = typename transition_table_digest::state_tuple;
        using action_tuple = typename transition_table_digest::action_tuple;
        using guard_tuple  = typename transition_table_digest::guard_tuple;
        using event_tuple  = typename transition_table_digest::event_tuple;

    public:
        fsm(context& ctx):
            ctx_(ctx),
            states_(detail::make_tuple<state_tuple>(ctx)),
            actions_(detail::make_tuple<action_tuple>(ctx)),
            guards_(detail::make_tuple<guard_tuple>(ctx)),
            state_transition_policy_{ctx},
            internal_transition_policy_{ctx}
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

        void process_event(const any_cref& event)
        {
            if constexpr(Configuration::enable_event_queue)
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
        void process_event_once(const any_cref& event)
        {
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
            if constexpr(detail::tlu::contains<event_tuple, none>)
                if(processed)
                    while(process_event_in_transition_table_once(none{}));
        }

        //Try and trigger one transition
        bool process_event_in_transition_table_once(const any_cref& event)
        {
            bool processed = false;

            visit_active_state
            (
                [&](auto& active_state)
                {
                    using active_state_t = std::decay_t<decltype(active_state)>;
                    using helper_t =
                        process_event_in_transition_table_once_helper
                        <
                            active_state_t
                        >
                    ;

                    const auto helper = helper_t
                    {
                        *this,
                        active_state,
                        event,
                        processed
                    };

                    //For each row
                    detail::tlu::for_each<transition_table>(helper);
                }
            );

            return processed;
        }

        template<class ActiveState>
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

                //If the transition start state is the active state or any
                if constexpr
                (
                    std::is_same_v<transition_start_state, ActiveState> ||
                    std::is_same_v<transition_start_state, any>
                )
                {
                    //Make sure we don't trigger more than one transition
                    if(processed)
                        return;

                    if(event.is<transition_event>())
                    {
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
                            ActiveState,
                            transition_target_state,
                            transition_action,
                            transition_guard
                        >
                        {
                            active_state,
                            event,
                            ptarget_state,
                            paction,
                            pguard,
                            self.active_state_index_,
                            processed,
                            target_state_index
                        };

                        //Perform the transition
                        self.state_transition_policy_(helper);
                    }
                }
            }

            fsm& self;
            ActiveState& active_state;
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
                    internal_transition_policy_(helper);
                }
            );
        }

    private:
        context& ctx_;
        state_tuple states_;
        action_tuple actions_;
        guard_tuple guards_;
        state_transition_policy state_transition_policy_;
        internal_transition_policy internal_transition_policy_;

        int active_state_index_ = 0;
        bool processing_event_ = false;
        std::queue<any_copy> deferred_events_;
};

} //namespace

#endif

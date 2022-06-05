//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_REGION_HPP
#define AWESM_DETAIL_REGION_HPP

#include "../region_configuration.hpp"
#include "../none.hpp"
#include "call_member.hpp"
#include "resolve_transition_table.hpp"
#include "transition_table_digest.hpp"
#include "alternative_lazy.hpp"
#include "any_container.hpp"
#include "ignore_unused.hpp"
#include "tlu/apply.hpp"
#include <type_traits>

namespace awesm::detail
{

template<class Configuration, class PrivateConfiguration>
class region
{
    public:
        static_assert
        (
            std::is_base_of_v<awesm::region_configuration, Configuration>,
            "Given configuration type must inherit from awesm::region_configuration."
        );

        template<class Sm, class Context>
        explicit region(Sm& sm, Context& context):
            states_(sm, context),
            actions_(sm, context),
            guards_(sm, context),
            exception_handler_{sm, context},
            state_transition_hook_set_{sm, context}
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
            return given_state_index == active_state_index_;
        }

        template<class Event>
        void start(const Event& event)
        {
            safe_call
            (
                [&]
                {
                    detail::call_on_entry
                    (
                        &states_.get(static_cast<initial_state_t*>(nullptr)),
                        &event,
                        0
                    );
                }
            );
        }

        void start()
        {
            start(none{});
        }

        template<class Event>
        void stop(const Event& event)
        {
            return detail::tlu::apply
            <
                state_tuple_t,
                stop_helper
            >::call(*this, event);
        }

        void stop()
        {
            stop(none{});
        }

        template<class Event>
        void process_event(const Event& event)
        {
            if constexpr(Configuration::enable_in_state_internal_transitions)
            {
                process_event_in_active_state(event);
            }

            const bool processed = process_event_in_transition_table_once(event);

            //Anonymous transitions
            if constexpr(transition_table_digest_t::has_none_events)
            {
                if(processed)
                {
                    while(process_event_in_transition_table_once(none{})){}
                }
            }
            else
            {
                detail::ignore_unused(processed);
            }
        }

    private:
        using unresolved_transition_table_t =
            typename Configuration::transition_table
        ;
        using exception_handler_t =
            typename PrivateConfiguration::exception_handler
        ;
        using state_transition_hook_set_t =
            typename PrivateConfiguration::state_transition_hook_set
        ;

        using transition_table_digest_t =
            detail::transition_table_digest<unresolved_transition_table_t>
        ;
        using state_tuple_t = typename transition_table_digest_t::state_tuple;
        using action_tuple_t = typename transition_table_digest_t::action_tuple;
        using guard_tuple_t = typename transition_table_digest_t::guard_tuple;

        using initial_state_t = detail::tlu::at<state_tuple_t, 0>;

        /*
        Calling detail::resolve_transition_table<> isn't free. We need
        alternative_lazy to avoid the call when it's unnecessary (i.e. when
        there's no pattern-source-state in the transition table).
        */
        struct unresolved_transition_table_holder
        {
            template<class = void>
            using type = unresolved_transition_table_t;
        };
        struct resolved_transition_table_holder
        {
            template<class = void>
            using type = detail::resolve_transition_table
            <
                unresolved_transition_table_t,
                state_tuple_t
            >;
        };
        using transition_table_t = detail::alternative_lazy
        <
            transition_table_digest_t::has_source_state_patterns,
            resolved_transition_table_holder,
            unresolved_transition_table_holder
        >;

        //Used to call client code
        template<class F>
        void safe_call(F&& f)
        {
            try
            {
                f();
            }
            catch(...)
            {
                exception_handler_.on_exception(std::current_exception());
            }
        }

        //Used to call client code
        template<class F>
        bool safe_call_or_false(F&& f)
        {
            try
            {
                return f();
            }
            catch(...)
            {
                exception_handler_.on_exception(std::current_exception());
                return false;
            }
        }

        //Try and trigger one transition
        template<class Event>
        bool process_event_in_transition_table_once(const Event& event)
        {
            return detail::tlu::apply
            <
                transition_table_t,
                transition_table_event_processor
            >::process(*this, event);
        }

        template<class... Rows>
        struct transition_table_event_processor
        {
            template<class Event>
            static bool process(region& reg, const Event& event)
            {
                return
                (
                    transition_table_row_event_processor<Rows>::process
                    (
                        reg,
                        &event
                    ) || ...
                );
            }
        };

        //Processes events against one row of the (resolved) transition table
        template<class Row>
        struct transition_table_row_event_processor
        {
            static bool process
            (
                region& reg,
                const typename Row::event_type* const pevent
            )
            {
                return reg.process_event_in_row<Row>(*pevent);
            }

            /*
            This void* trick allows for shorter build times.

            This alternative implementation takes more time to build:
                template<class Event>
                static bool process(sm&, const Event&)
                {
                    return false;
                }

            Using an if-constexpr in the process() function above is worse as
            well.
            */
            static bool process(region& /*reg*/, const void* /*pevent*/)
            {
                return false;
            }
        };

        template<class Row, class Event>
        bool process_event_in_row(const Event& event)
        {
            using source_state_t = typename Row::source_state_type;
            using target_state_t = typename Row::target_state_type;
            using action_t       = typename Row::action_type;
            using guard_t        = typename Row::guard_type;

            //Make sure the transition source state is the active state
            if(!is_active_state<source_state_t>())
            {
                return false;
            }

            const auto do_transition = [&](auto /*dummy*/)
            {
                constexpr auto is_internal_transition =
                    std::is_same_v<target_state_t, none>
                ;

                if constexpr(!is_internal_transition)
                {
                    state_transition_hook_set_.template before_transition
                    <
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);

                    detail::call_on_exit
                    (
                        &states_.get(static_cast<source_state_t*>(nullptr)),
                        &event,
                        0
                    );

                    active_state_index_ = detail::tlu::get_index
                    <
                        state_tuple_t,
                        target_state_t
                    >;
                }

                if constexpr(!std::is_same_v<action_t, none>)
                {
                    detail::call_execute
                    (
                        &actions_.get(static_cast<action_t*>(nullptr)),
                        &event
                    );
                }

                if constexpr(!is_internal_transition)
                {
                    detail::call_on_entry
                    (
                        &states_.get(static_cast<target_state_t*>(nullptr)),
                        &event,
                        0
                    );

                    state_transition_hook_set_.template after_transition
                    <
                        source_state_t,
                        Event,
                        target_state_t
                    >(event);
                }
            };

            if constexpr(std::is_same_v<guard_t, none>)
            {
                safe_call
                (
                    [&]
                    {
                        do_transition(0);
                    }
                );
                return true;
            }
            else
            {
                return safe_call_or_false
                (
                    [&]
                    {
                        if
                        (
                            !detail::call_check
                            (
                                &guards_.get(static_cast<guard_t*>(nullptr)),
                                &event
                            )
                        )
                        {
                            return false;
                        }

                        do_transition(0);
                        return true;
                    }
                );
            }
        }

        template<class... States>
        struct stop_helper
        {
            template<class Event>
            static void call(region& reg, const Event& event)
            {
                (reg.call_on_exit_of_active_state<States>(&event) || ...);
            }
        };

        template<class State, class Event>
        bool call_on_exit_of_active_state(const Event* pevent)
        {
            if(is_active_state<State>())
            {
                safe_call
                (
                    [&]
                    {
                        detail::call_on_exit
                        (
                            &states_.get(static_cast<State*>(nullptr)),
                            pevent,
                            0
                        );
                    }
                );
                return true;
            }
            return false;
        }

        /*
        Call active_state.on_event(event)
        */
        template<class Event>
        void process_event_in_active_state(const Event& event)
        {
            return detail::tlu::apply
            <
                state_tuple_t,
                active_state_event_processor
            >::process(*this, event);
        }

        //Processes internal events against all states
        template<class... States>
        struct active_state_event_processor
        {
            template<class Event>
            static void process(region& reg, const Event& event)
            {
                (reg.process_event_in_state<States>(&event) || ...);
            }
        };

        //Processes internal events against one state
        template<class State, class Event>
        auto process_event_in_state
        (
            const Event* pevent
        ) -> decltype(std::declval<State>().on_event(*pevent), bool())
        {
            if(is_active_state<State>())
            {
                safe_call
                (
                    [&]
                    {
                        states_.get(static_cast<State*>(nullptr)).on_event(*pevent);
                    }
                );
                return true;
            }
            return false;
        }

        template<class State>
        bool process_event_in_state(const void* /*pevent*/)
        {
            return false;
        }

        state_tuple_t states_;
        action_tuple_t actions_;
        guard_tuple_t guards_;
        exception_handler_t exception_handler_;
        state_transition_hook_set_t state_transition_hook_set_;

        int active_state_index_ = 0;
};

} //namespace

#endif

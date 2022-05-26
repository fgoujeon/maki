//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_HPP
#define AWESM_SM_HPP

#include "sm_configuration.hpp"
#include "none.hpp"
#include "detail/call_member.hpp"
#include "detail/resolve_transition_table.hpp"
#include "detail/transition_table_digest.hpp"
#include "detail/alternative_lazy.hpp"
#include "detail/any_container.hpp"
#include "detail/ignore_unused.hpp"
#include "detail/tlu/apply.hpp"
#include <queue>
#include <type_traits>

namespace awesm
{

namespace detail
{
    class false_at_destruction_setter
    {
        public:
            false_at_destruction_setter(bool& b):
                b_(b)
            {
            }

            false_at_destruction_setter(const false_at_destruction_setter&) = delete;
            false_at_destruction_setter(false_at_destruction_setter&&) = delete;
            false_at_destruction_setter& operator=(const false_at_destruction_setter&) = delete;
            false_at_destruction_setter& operator=(false_at_destruction_setter&&) = delete;

            ~false_at_destruction_setter()
            {
                b_ = false;
            }

        private:
            bool& b_;
    };
}

template<class Configuration>
class sm
{
    public:
        static_assert
        (
            std::is_base_of_v<sm_configuration, Configuration>,
            "Given configuration type must inherit from awesm::sm_configuration."
        );

        template<class Context>
        explicit sm(Context& context):
            states_(context, *this),
            actions_(context, *this),
            guards_(context, *this),
            exception_handler_{context, *this},
            pre_transition_event_handler_{context, *this}
        {
        }

        sm(const sm&) = delete;
        sm(sm&&) = delete;
        sm& operator=(const sm&) = delete;
        sm& operator=(sm&&) = delete;
        ~sm() = default;

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
        void process_event(const Event& event)
        {
            if constexpr(Configuration::enable_run_to_completion)
            {
                //Queue event processing in case of recursive call
                if(processing_event_)
                {
                    safe_call //copy constructor might throw
                    (
                        [&]
                        {
                            queued_event_processings_.emplace(*this, event);
                        }
                    );
                    return;
                }

                processing_event_ = true;
                auto _ = detail::false_at_destruction_setter{processing_event_};

                process_event_once(event);

                //Process deferred event processings
                while(!queued_event_processings_.empty())
                {
                    queued_event_processings_.front()();
                    queued_event_processings_.pop();
                }
            }
            else
            {
                process_event_once(event);
            }
        }

    private:
        using unresolved_transition_table_t =
            typename Configuration::transition_table
        ;
        using exception_handler_t =
            typename Configuration::template exception_handler<sm>
        ;
        using pre_transition_event_handler_t =
            typename Configuration::pre_transition_event_handler
        ;

        using transition_table_digest_t =
            detail::transition_table_digest<unresolved_transition_table_t>
        ;
        using state_tuple_t = typename transition_table_digest_t::state_tuple;
        using action_tuple_t = typename transition_table_digest_t::action_tuple;
        using guard_tuple_t = typename transition_table_digest_t::guard_tuple;

        class event_processing
        {
            public:
                template<class Event>
                event_processing(sm& machine, const Event& event):
                    sm_(machine),
                    event_(event),
                    pprocess_event_
                    (
                        [](sm& machine, const event_storage_t& event)
                        {
                            machine.process_event_once(event.get<Event>());
                        }
                    )
                {
                }

                event_processing(const event_processing&) = delete;
                event_processing(event_processing&&) = delete;
                ~event_processing() = default;
                event_processing& operator=(const event_processing&) = delete;
                event_processing& operator=(event_processing&&) = delete;

                void operator()()
                {
                    (*pprocess_event_)(sm_, event_);
                }

            private:
                static constexpr auto small_event_size = 16;
                using event_storage_t = detail::any_container<small_event_size>;

                sm& sm_;
                event_storage_t event_;
                void(*pprocess_event_)(sm&, const event_storage_t&) = nullptr;
        };

        /*
        Calling detail::resolve_transition_table<> isn't free. We need
        alternative_lazy to avoid the call when it's unnecessary (i.e. when
        there's no pattern-start-state in the transition table).
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

        struct function_queue_holder
        {
            template<class = void>
            using type = std::queue<event_processing>;
        };
        struct empty_holder
        {
            template<class = void>
            struct type{};
        };
        using queued_event_processing_storage_t = detail::alternative_lazy
        <
            Configuration::enable_run_to_completion,
            function_queue_holder,
            empty_holder
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

        template<class Event>
        void process_event_once(const Event& event)
        {
            safe_call
            (
                [&]
                {
                    detail::call_on_event
                    (
                        &pre_transition_event_handler_,
                        &event,
                        0
                    );
                }
            );

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
            static bool process(sm& machine, const Event& event)
            {
                return (transition_table_row_event_processor<Rows>::process(machine, &event) || ...);
            }
        };

        //Processes events against one row of the (resolved) transition table
        template<class Row>
        struct transition_table_row_event_processor
        {
            static bool process
            (
                sm& machine,
                const typename Row::event_type* const pevent
            )
            {
                return machine.process_event_in_row<Row>(*pevent);
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
            static bool process(sm& /*machine*/, const void* /*pevent*/)
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

            const auto get_target_state_ptr = [&]
            {
                if constexpr(std::is_same_v<target_state_t, none>)
                {
                    return nullptr;
                }
                else
                {
                    return &states_.get(static_cast<target_state_t*>(nullptr));
                }
            };

            const auto do_transition = [&]
            {
                if constexpr(!std::is_same_v<target_state_t, none>)
                {
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

                if constexpr(!std::is_same_v<target_state_t, none>)
                {
                    detail::call_on_entry
                    (
                        get_target_state_ptr(),
                        &event,
                        0
                    );
                }
            };

            return safe_call_or_false
            (
                [&]
                {
                    if constexpr(std::is_same_v<guard_t, none>)
                    {
                        do_transition();
                        return true;
                    }
                    else
                    {
                        if
                        (
                            detail::call_check
                            (
                                &guards_.get(static_cast<guard_t*>(nullptr)),
                                &event
                            )
                        )
                        {
                            do_transition();
                            return true;
                        }
                    }
                    return false;
                }
            );
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
            static void process(sm& machine, const Event& event)
            {
                (machine.process_event_in_state<States>(&event) || ...);
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
        pre_transition_event_handler_t pre_transition_event_handler_;

        int active_state_index_ = 0;
        bool processing_event_ = false;
        queued_event_processing_storage_t queued_event_processings_;
};

} //namespace

#endif

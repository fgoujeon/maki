//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_HPP
#define AWESM_SM_HPP

#include "sm_configuration.hpp"
#include "none.hpp"
#include "detail/region.hpp"
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
            region_{*this, context},
            exception_handler_{*this, context},
            pre_transition_event_handler_{*this, context}
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
            return region_.template is_active_state<State>();
        }

        template<class Event>
        void start(const Event& event)
        {
            region_.start(event);
        }

        void start()
        {
            region_.start();
        }

        template<class Event>
        void stop(const Event& event)
        {
            region_.stop(event);
        }

        void stop()
        {
            region_.stop();
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
        using exception_handler_t =
            typename Configuration::template exception_handler<sm>
        ;
        using pre_transition_event_handler_t =
            typename Configuration::pre_transition_event_handler
        ;
        using state_transition_hook_set_t =
            typename Configuration::template state_transition_hook_set<sm>
        ;

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

        struct region_configuration: awesm::region_configuration
        {
            using transition_table =
                typename Configuration::transition_table
            ;

            template<class Sm>
            using exception_handler =
                typename Configuration::template exception_handler<Sm>
            ;

            template<class Sm>
            using state_transition_hook_set =
                typename Configuration::template state_transition_hook_set<Sm>
            ;

            static constexpr auto enable_in_state_internal_transitions =
                Configuration::enable_in_state_internal_transitions
            ;
        };

        using region_t = detail::region<sm, region_configuration>;

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
                        &event
                    );
                }
            );

            region_.process_event(event);
        }

        region_t region_;

        exception_handler_t exception_handler_;
        pre_transition_event_handler_t pre_transition_event_handler_;

        bool processing_event_ = false;
        queued_event_processing_storage_t queued_event_processings_;
};

} //namespace

#endif

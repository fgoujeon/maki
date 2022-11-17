//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_HPP
#define AWESM_SM_HPP

#include "sm_conf.hpp"
#include "null.hpp"
#include "detail/composite_state_wrapper.hpp"
#include "detail/region_tuple.hpp"
#include "detail/event_processing_type.hpp"
#include "detail/alternative_lazy.hpp"
#include "detail/any_container.hpp"
#include "detail/sm_path.hpp"
#include <queue>
#include <type_traits>

namespace awesm
{

template<class Def>
class sm
{
    public:
        using conf = typename Def::conf;
        using context_t = typename conf::context_t;

        explicit sm(context_t& context):
            def_(*this, context),
            region_tuple_{*this, context}
        {
        }

        sm(const sm&) = delete;
        sm(sm&&) = delete;
        sm& operator=(const sm&) = delete;
        sm& operator=(sm&&) = delete;
        ~sm() = default;

        template<class State, int RegionIndex = 0>
        const auto& get_state() const
        {
            return region_tuple_.template get_state<State, RegionIndex>();
        }

        [[nodiscard]] bool is_running() const
        {
            return !is_active_state<detail::null_state>();
        }

        template<class State, int RegionIndex = 0>
        [[nodiscard]] bool is_active_state() const
        {
            return region_tuple_.template is_active_state<State, RegionIndex>();
        }

        template<class Event = null>
        void start(const Event& event = {})
        {
            process_event_2<detail::event_processing_type::start>(event);
        }

        template<class Event = null>
        void stop(const Event& event = {})
        {
            process_event_2<detail::event_processing_type::stop>(event);
        }

        template<class Event>
        void process_event(const Event& event)
        {
            process_event_2<detail::event_processing_type::event>(event);
        }

        static decltype(auto) get_pretty_name()
        {
            return awesm::get_pretty_name<Def>();
        }

    private:
        //Let regions access definition and option list
        template<class Sm, class RegionPath, class TransitionTable>
        friend class detail::region;

        using conf_t = typename Def::conf;
        using transition_table_list_t = typename conf_t::transition_table_list_t;

        using path_t = detail::sm_path<region_path<>, sm>;

        class event_processing
        {
            public:
                template<class Event, detail::event_processing_type ProcessingType>
                event_processing
                (
                    sm& machine,
                    const Event& event,
                    std::integral_constant<detail::event_processing_type, ProcessingType> /*unused*/
                ):
                    sm_(machine),
                    event_(event),
                    pprocess_event_
                    (
                        [](sm& mach, const event_storage_t& evt)
                        {
                            mach.process_event_once<ProcessingType>(evt.get<Event>());
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

        struct queue_holder
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
            detail::tlu::contains<conf_t, sm_options::disable_run_to_completion>,
            empty_holder,
            queue_holder
        >;

        template<detail::event_processing_type ProcessingType, class Event>
        void process_event_2(const Event& event)
        {
            if constexpr(!detail::tlu::contains<conf_t, sm_options::disable_run_to_completion>)
            {
                //Queue event processing in case of recursive call
                if(processing_event_)
                {
                    safe_call //copy constructor might throw
                    (
                        [&]
                        {
                            queued_event_processings_.emplace
                            (
                                *this,
                                event,
                                std::integral_constant
                                <
                                    detail::event_processing_type,
                                    ProcessingType
                                >{}
                            );
                        }
                    );
                    return;
                }

                processing_event_ = true;

                process_event_once<ProcessingType>(event);

                //Process queued event processings
                while(!queued_event_processings_.empty())
                {
                    queued_event_processings_.front()();
                    queued_event_processings_.pop();
                }

                processing_event_ = false;
            }
            else
            {
                process_event_once<ProcessingType>(event);
            }
        }

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
                process_exception(std::current_exception());
            }
        }

        void process_exception(const std::exception_ptr& eptr)
        {
            if constexpr(detail::tlu::contains<conf_t, sm_options::on_exception>)
            {
                def_.on_exception(eptr);
            }
            else
            {
                process_event(eptr);
            }
        }

        template<detail::event_processing_type ProcessingType, class Event>
        void process_event_once(const Event& event)
        {
            if constexpr
            (
                ProcessingType == detail::event_processing_type::event &&
                detail::tlu::contains<conf_t, sm_options::on_event>
            )
            {
                safe_call
                (
                    [&]
                    {
                        def_.on_event(event);
                    }
                );
            }

            if constexpr(ProcessingType == detail::event_processing_type::start)
            {
                region_tuple_.start(event);
            }
            else if constexpr(ProcessingType == detail::event_processing_type::stop)
            {
                region_tuple_.stop(event);
            }
            else
            {
                region_tuple_.process_event(event);
            }
        }

        detail::sm_object_holder<Def> def_;
        detail::region_tuple<sm, path_t, transition_table_list_t> region_tuple_;

        bool processing_event_ = false;
        queued_event_processing_storage_t queued_event_processings_;
};

} //namespace

#endif

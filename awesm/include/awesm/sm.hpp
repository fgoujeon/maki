//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_HPP
#define AWESM_SM_HPP

#include "sm_conf.hpp"
#include "region_path.hpp"
#include "detail/noinline.hpp"
#include "detail/subsm.hpp"
#include "detail/alternative.hpp"
#include "detail/function_queue.hpp"
#include "detail/tlu.hpp"
#include "detail/type_tag.hpp"
#include "detail/overload_priority.hpp"
#include <type_traits>

namespace awesm
{

namespace detail
{
    enum class sm_operation
    {
        start,
        stop,
        process_event
    };
}

template<class Def>
class sm
{
public:
    using def_type = Def;
    using conf = typename Def::conf;
    using context_type = detail::tlu::get_f_t<conf, detail::sm_option::context>;

    template<class... ContextArgs>
    explicit sm(ContextArgs&&... ctx_args):
        subsm_(*this, std::forward<ContextArgs>(ctx_args)...)
    {
        if constexpr(detail::tlu::get_f_t<conf, detail::sm_option::auto_start>::value)
        {
            //start
            process_event_now_impl<detail::sm_operation::start>(events::start{});
        }
    }

    sm(const sm&) = delete;
    sm(sm&&) = delete;
    sm& operator=(const sm&) = delete;
    sm& operator=(sm&&) = delete;
    ~sm() = default;

    Def& get_def()
    {
        return subsm_.get_def();
    }

    const Def& get_def() const
    {
        return subsm_.get_def();
    }

    context_type& get_context()
    {
        return subsm_.get_context();
    }

    const context_type& get_context() const
    {
        return subsm_.get_context();
    }

    template<class RegionPath>
    [[nodiscard]] bool is_running() const
    {
        return subsm_.template is_running<RegionPath>();
    }

    template<bool Dummy = true>
    [[nodiscard]] bool is_running() const
    {
        return subsm_.is_running();
    }

    template<class StateRegionPath, class State>
    [[nodiscard]] bool is_active_state() const
    {
        return subsm_.template is_active_state<StateRegionPath, State>();
    }

    template<class State>
    [[nodiscard]] bool is_active_state() const
    {
        return subsm_.template is_active_state<State>();
    }

    template<class Event = events::start>
    void start(const Event& event = {})
    {
        process_event_2<detail::sm_operation::start>(event);
    }

    template<class Event = events::stop>
    void stop(const Event& event = {})
    {
        process_event_2<detail::sm_operation::stop>(event);
    }

    template<class Event>
    void process_event(const Event& event)
    {
        process_event_2<detail::sm_operation::process_event>(event);
    }

    template<class Event>
    AWESM_NOINLINE void queue_event(const Event& event)
    {
        queue_event_impl<detail::sm_operation::process_event>(event);
    }

private:
    struct any_event_queue_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::function_queue
        <
            sm&,
            detail::tlu::get_f_t<conf, detail::sm_option::small_event_max_size>::value,
            detail::tlu::get_f_t<conf, detail::sm_option::small_event_max_align>::value
        >;
    };
    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };
    using any_event_queue_type = typename detail::alternative_t
    <
        detail::tlu::get_f_t<conf, detail::sm_option::run_to_completion>::value,
        any_event_queue_holder,
        empty_holder
    >::template type<>;

    template<detail::sm_operation Operation, class Event>
    void process_event_2(const Event& event)
    {
        if constexpr(detail::tlu::get_f_t<conf, detail::sm_option::run_to_completion>::value)
        {
            if(!processing_event_) //If call is not recursive
            {
                process_event_now_impl<Operation>(event);
            }
            else
            {
                //Queue event in case of recursive call
                queue_event_impl<Operation>(event);
            }
        }
        else
        {
            process_event_once<Operation>(event);
        }
    }

    template<detail::sm_operation Operation, class Event>
    void process_event_now_impl(const Event& event)
    {
        if constexpr(detail::tlu::get_f_t<conf, detail::sm_option::run_to_completion>::value)
        {
            processing_event_ = true;

            process_event_once<Operation>(event);

            //Process queued events, if any
            event_queue_.invoke_and_pop_all(*this);

            processing_event_ = false;
        }
        else
        {
            process_event_once<Operation>(event);
        }
    }

    template<detail::sm_operation Operation, class Event>
    void queue_event_impl(const Event& event)
    {
        if constexpr(std::is_nothrow_copy_constructible_v<Event>)
        {
            event_queue_.template push<any_event_visitor<Operation>>(event);
        }
        else
        {
            try
            {
                event_queue_.template push<any_event_visitor<Operation>>(event);
            }
            catch(...)
            {
                process_exception(std::current_exception());
            }
        }
    }

    template<detail::sm_operation Operation>
    struct any_event_visitor
    {
        template<class Event>
        static void call(const Event& event, sm& self)
        {
            self.process_event_once<Operation>(event);
        }
    };

    void process_exception(const std::exception_ptr& eptr)
    {
        if constexpr(detail::tlu::get_f_t<conf, detail::sm_option::on_exception>::value)
        {
            get_def().on_exception(eptr);
        }
        else
        {
            queue_event(events::exception{eptr});
        }
    }

    template<detail::sm_operation Operation, class Event>
    void process_event_once(const Event& event)
    {
        try
        {
            if constexpr(Operation == detail::sm_operation::start)
            {
                subsm_.on_entry(event);
            }
            else if constexpr(Operation == detail::sm_operation::stop)
            {
                subsm_.on_exit(event);
            }
            else
            {
                if constexpr(detail::tlu::get_f_t<conf, detail::sm_option::on_unprocessed>::value)
                {
                    if(!subsm_.on_event(event))
                    {
                        get_def().on_unprocessed(event);
                    }
                }
                else
                {
                    subsm_.on_event(event);
                }
            }
        }
        catch(...)
        {
            process_exception(std::current_exception());
        }
    }

    detail::subsm<Def, void> subsm_;
    bool processing_event_ = false;
    any_event_queue_type event_queue_;
};

} //namespace

#endif

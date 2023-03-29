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
#include "detail/function_queue.hpp"
#include "detail/tlu.hpp"
#include "detail/type.hpp"
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
    using context_type = detail::option_t<conf, detail::option_id::context>;

    static_assert(detail::is_root_sm_conf_v<conf>, "Root SM must define a using conf = sm_conf::...");

    template<class... ContextArgs>
    explicit sm(ContextArgs&&... ctx_args):
        subsm_(*this, std::forward<ContextArgs>(ctx_args)...)
    {
        if constexpr(detail::option_v<conf, detail::option_id::auto_start>)
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

    template<class RegionPath, class State>
    State& get_state()
    {
        return subsm_.template get_state<RegionPath, State>();
    }

    template<class RegionPath, class State>
    const State& get_state() const
    {
        return subsm_.template get_state<RegionPath, State>();
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
        static_assert(detail::option_v<conf, detail::option_id::run_to_completion>);
        try
        {
            queue_event_impl<detail::sm_operation::process_event>(event);
        }
        catch(...)
        {
            process_exception(std::current_exception());
        }
    }

    void process_queued_events()
    {
        if(!processing_event_)
        {
            auto guard = processing_event_guard{*this};
            try
            {
                event_queue_.invoke_and_pop_all(*this);
            }
            catch(...)
            {
                process_exception(std::current_exception());
            }
        }
    }

private:
    class processing_event_guard
    {
    public:
        processing_event_guard(sm& self):
            self_(self)
        {
            self_.processing_event_ = true;
        }

        processing_event_guard(const processing_event_guard&) = delete;
        processing_event_guard(processing_event_guard&&) = delete;
        processing_event_guard& operator=(const processing_event_guard&) = delete;
        processing_event_guard& operator=(processing_event_guard&&) = delete;

        ~processing_event_guard()
        {
            self_.processing_event_ = false;
        }

    private:
        sm& self_;
    };

    struct any_event_queue_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        using type = detail::function_queue
        <
            sm&,
            detail::option_v<conf, detail::option_id::small_event_max_size>,
            detail::option_v<conf, detail::option_id::small_event_max_align>
        >;
    };
    struct empty_holder
    {
        template<bool = true> //Dummy template for lazy evaluation
        struct type{};
    };
    using any_event_queue_type = typename std::conditional_t
    <
        detail::option_v<conf, detail::option_id::run_to_completion>,
        any_event_queue_holder,
        empty_holder
    >::template type<>;

    template<detail::sm_operation Operation, class Event>
    void process_event_2(const Event& event)
    {
        try
        {
            if constexpr(detail::option_v<conf, detail::option_id::run_to_completion>)
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
        catch(...)
        {
            process_exception(std::current_exception());
        }
    }

    template<detail::sm_operation Operation, class Event>
    void process_event_now_impl(const Event& event)
    {
        if constexpr(detail::option_v<conf, detail::option_id::run_to_completion>)
        {
            auto guard = processing_event_guard{*this};

            process_event_once<Operation>(event);

            //Process queued events, if any
            event_queue_.invoke_and_pop_all(*this);
        }
        else
        {
            process_event_once<Operation>(event);
        }
    }

    template<detail::sm_operation Operation, class Event>
    void queue_event_impl(const Event& event)
    {
        event_queue_.template push<any_event_visitor<Operation>>(event);
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
        if constexpr(detail::option_v<conf, detail::option_id::on_exception>)
        {
            get_def().on_exception(eptr);
        }
        else
        {
            process_event(events::exception{eptr});
        }
    }

    template<detail::sm_operation Operation, class Event>
    void process_event_once(const Event& event)
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
            if constexpr(detail::option_v<conf, detail::option_id::on_unprocessed>)
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

    detail::subsm<Def, void> subsm_;
    bool processing_event_ = false;
    any_event_queue_type event_queue_;
};

} //namespace

#endif

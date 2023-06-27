//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_REF_HPP
#define AWESM_SM_REF_HPP

#include "sm_ref_conf.hpp"
#include "sm_fwd.hpp"
#include "detail/conf.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    template<class... Events>
    class sm_ref_event_impl;

    template<class Event, class... Events>
    class sm_ref_event_impl<Event, Events...>: sm_ref_event_impl<Events...>
    {
    public:
        template<class SmDef>
        sm_ref_event_impl(sm<SmDef>& machine):
            sm_ref_event_impl<Events...>{machine},
            pprocess_event_
            {
                [](void* const vpsm, const Event& event)
                {
                    using sm_t = sm<SmDef>;
                    const auto psm = reinterpret_cast<sm_t*>(vpsm); //NOLINT
                    psm->process_event(event);
                }
            },
            penqueue_event_
            {
                [](void* const vpsm, const Event& event)
                {
                    using sm_t = sm<SmDef>;
                    const auto psm = reinterpret_cast<sm_t*>(vpsm); //NOLINT
                    psm->enqueue_event(event);
                }
            }
        {
        }

        using sm_ref_event_impl<Events...>::process_event;

        void process_event(const Event& event)
        {
            (*pprocess_event_)(get_vpsm(), event);
        }

        using sm_ref_event_impl<Events...>::enqueue_event;

        void enqueue_event(const Event& event)
        {
            (*penqueue_event_)(get_vpsm(), event);
        }

    protected:
        using sm_ref_event_impl<Events...>::get_vpsm;

    private:
        void(*pprocess_event_)(void*, const Event&) = nullptr;
        void(*penqueue_event_)(void*, const Event&) = nullptr;
    };

    template<>
    class sm_ref_event_impl<>
    {
    public:
        template<class SmDef>
        sm_ref_event_impl(sm<SmDef>& machine):
            vpsm_(&machine)
        {
        }

        void process_event()
        {
        }

        void enqueue_event()
        {
        }

    protected:
        [[nodiscard]] void* get_vpsm() const
        {
            return vpsm_;
        }

    private:
        void* vpsm_ = nullptr;
    };
}

/*
sm_ref is a type-erasing container for a reference to a sm of any type.
It exposes the process_event() member function of the held sm.
*/
template<class Def>
class sm_ref
{
public:
    using conf = typename Def::conf;

    template<class SmDef>
    sm_ref(sm<SmDef>& machine):
        impl_{machine}
    {
    }

    sm_ref(const sm_ref&) noexcept = default;
    sm_ref(sm_ref&&) noexcept = default;
    sm_ref& operator=(const sm_ref&) noexcept = default;
    sm_ref& operator=(sm_ref&&) noexcept = default;
    ~sm_ref() = default;

    template<class Event>
    void process_event(const Event& event)
    {
        static_assert
        (
            detail::tlu::contains_v
            <
                detail::option_t<conf, detail::option_id::events>,
                Event
            >,
            "Given event type must be part of the 'events' option template argument list"
        );
        impl_.process_event(event);
    }

    template<class Event>
    void enqueue_event(const Event& event)
    {
        static_assert
        (
            detail::tlu::contains_v
            <
                detail::option_t<conf, detail::option_id::events>,
                Event
            >,
            "Given event type must be part of the 'events' option template argument list"
        );
        impl_.enqueue_event(event);
    }

private:
    using event_type_list = detail::option_t<conf, detail::option_id::events>;

    using event_impl_type = detail::tlu::apply_t
    <
        event_type_list,
        detail::sm_ref_event_impl
    >;

    event_impl_type impl_;
};

template<class... Events>
struct sm_ref_e_def
{
    using conf = sm_ref_conf
        ::events<Events...>
    ;
};

template<class... Events>
using sm_ref_e = sm_ref<sm_ref_e_def<Events...>>;

} //namespace

#endif

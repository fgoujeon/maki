//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_FSM_REF_HPP
#define FGFSM_FSM_REF_HPP

#include "detail/type_list.hpp"
#include "detail/tlu.hpp"

namespace fgfsm
{

namespace detail
{
    template<class... Events>
    struct fsm_ref_impl;

    template<class Event, class... Events>
    struct fsm_ref_impl<Event, Events...>: fsm_ref_impl<Events...>
    {
        template<class Fsm>
        fsm_ref_impl(Fsm& sm):
            fsm_ref_impl<Events...>{sm},
            pprocess_event
            {
                [](void* const vpfsm, const Event& event)
                {
                    const auto pfsm = reinterpret_cast<Fsm*>(vpfsm);
                    pfsm->process_event(event);
                }
            }
        {
        }

        using fsm_ref_impl<Events...>::process_event;

        void process_event(const Event& event)
        {
            (*pprocess_event)(this->vpfsm, event);
        }

        void(*pprocess_event)(void*, const Event&) = nullptr;
    };

    template<>
    struct fsm_ref_impl<>
    {
        template<class Fsm>
        fsm_ref_impl(Fsm& sm):
            vpfsm(&sm)
        {
        }

        void process_event()
        {
        }

        void* vpfsm = nullptr;
    };
}

/*
fsm_ref is a type-erasing container for a reference to an fsm of any type.
It exposes the process_event() member function of the held fsm.
*/
template<class... Events>
class fsm_ref
{
    public:
        template<class Fsm>
        fsm_ref(Fsm& sm):
            impl_{sm}
        {
        }

        template<class Event>
        void process_event(const Event& event)
        {
            static_assert
            (
                detail::tlu::contains<detail::type_list<Events...>, Event>,
                "Given event type must be part of fsm_ref template argument list"
            );
            impl_.process_event(event);
        }

    private:
        detail::fsm_ref_impl<Events...> impl_;
};

} //namespace

#endif

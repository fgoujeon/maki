//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_FSM_REF_HPP
#define AWESM_FSM_REF_HPP

#include "fsm.hpp"
#include "detail/type_list.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    template<class... Events>
    class fsm_ref_impl;

    template<class Event, class... Events>
    class fsm_ref_impl<Event, Events...>: fsm_ref_impl<Events...>
    {
        public:
            template<class FsmConfiguration>
            fsm_ref_impl(fsm<FsmConfiguration>& sm):
                fsm_ref_impl<Events...>{sm},
                pprocess_event_
                {
                    [](void* const vpfsm, const Event& event)
                    {
                        using fsm_t = fsm<FsmConfiguration>;
                        const auto pfsm = reinterpret_cast<fsm_t*>(vpfsm); //NOLINT
                        pfsm->process_event(event);
                    }
                }
            {
            }

            using fsm_ref_impl<Events...>::process_event;

            void process_event(const Event& event)
            {
                (*pprocess_event_)(get_vpfsm(), event);
            }

        protected:
            using fsm_ref_impl<Events...>::get_vpfsm;

        private:
            void(*pprocess_event_)(void*, const Event&) = nullptr;
    };

    template<>
    class fsm_ref_impl<>
    {
        public:
            template<class FsmConfiguration>
            fsm_ref_impl(fsm<FsmConfiguration>& sm):
                vpfsm_(&sm)
            {
            }

            void process_event()
            {
            }

        protected:
            [[nodiscard]] void* get_vpfsm() const
            {
                return vpfsm_;
            }

        private:
            void* vpfsm_ = nullptr;
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
        template<class FsmConfiguration>
        fsm_ref(fsm<FsmConfiguration>& sm):
            impl_{sm}
        {
        }

        fsm_ref(const fsm_ref&) noexcept = default;
        fsm_ref(fsm_ref&&) noexcept = default;
        fsm_ref& operator=(const fsm_ref&) noexcept = default;
        fsm_ref& operator=(fsm_ref&&) noexcept = default;
        ~fsm_ref() = default;

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

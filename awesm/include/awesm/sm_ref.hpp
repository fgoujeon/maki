//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_SM_REF_HPP
#define AWESM_SM_REF_HPP

#include "sm_fwd.hpp"
#include "detail/type_list.hpp"
#include "detail/tlu.hpp"

namespace awesm
{

namespace detail
{
    template<class... Events>
    class sm_ref_impl;

    template<class Event, class... Events>
    class sm_ref_impl<Event, Events...>: sm_ref_impl<Events...>
    {
        public:
            template<class SmDef>
            sm_ref_impl(sm<SmDef>& machine):
                sm_ref_impl<Events...>{machine},
                pprocess_event_
                {
                    [](void* const vpsm, const Event& event)
                    {
                        using sm_t = sm<SmDef>;
                        const auto psm = reinterpret_cast<sm_t*>(vpsm); //NOLINT
                        psm->process_event(event);
                    }
                }
            {
            }

            using sm_ref_impl<Events...>::process_event;

            void process_event(const Event& event)
            {
                (*pprocess_event_)(get_vpsm(), event);
            }

        protected:
            using sm_ref_impl<Events...>::get_vpsm;

        private:
            void(*pprocess_event_)(void*, const Event&) = nullptr;
    };

    template<>
    class sm_ref_impl<>
    {
        public:
            template<class SmDef>
            sm_ref_impl(sm<SmDef>& machine):
                vpsm_(&machine)
            {
            }

            void process_event()
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
template<class... Events>
class sm_ref
{
    public:
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
                detail::tlu::contains_v<detail::type_list<Events...>, Event>,
                "Given event type must be part of sm_ref template argument list"
            );
            impl_.process_event(event);
        }

    private:
        detail::sm_ref_impl<Events...> impl_;
};

} //namespace

#endif

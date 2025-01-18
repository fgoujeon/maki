//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_ref class template
*/

#ifndef MAKI_MACHINE_REF_HPP
#define MAKI_MACHINE_REF_HPP

#include "machine_ref_conf.hpp"
#include "machine_fwd.hpp"
#include "detail/tlu/apply.hpp"
#include "detail/tlu/contains.hpp"
#include <type_traits>

namespace maki
{

namespace detail
{
    template<class... Events>
    class machine_ref_event_impl;

    template<class Event, class... Events>
    class machine_ref_event_impl<Event, Events...>: machine_ref_event_impl<Events...>
    {
    public:
        template<const auto& MachineConf>
        machine_ref_event_impl(machine<MachineConf>& mach):
            machine_ref_event_impl<Events...>{mach},
            pprocess_event_
            {
                [](void* const vpsm, const Event& evt)
                {
                    using machine_t = machine<MachineConf>;
                    const auto psm = reinterpret_cast<machine_t*>(vpsm); //NOLINT
                    psm->process_event(evt);
                }
            }
        {
        }

        using machine_ref_event_impl<Events...>::process_event;

        void process_event(const Event& evt) const
        {
            (*pprocess_event_)(get_vpsm(), evt);
        }

    protected:
        using machine_ref_event_impl<Events...>::get_vpsm;

    private:
        void(*pprocess_event_)(void*, const Event&) = nullptr;
    };

    template<>
    class machine_ref_event_impl<>
    {
    public:
        template<const auto& MachineConf>
        machine_ref_event_impl(machine<MachineConf>& mach):
            vpsm_(&mach)
        {
        }

        void process_event() const
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

/**
@brief A type-erasing container for a reference to a @ref machine of any type.

It exposes the process_event() member function of the held machine.
*/
template<const auto& Conf>
class machine_ref
{
public:
    template<const auto& MachineConf>
    machine_ref(machine<MachineConf>& mach):
        impl_{mach}
    {
    }

    machine_ref(const machine_ref&) noexcept = default;
    machine_ref(machine_ref&&) noexcept = default;
    machine_ref& operator=(const machine_ref&) noexcept = default;
    machine_ref& operator=(machine_ref&&) noexcept = default;
    ~machine_ref() = default;

    template<class Event>
    void process_event(const Event& evt) const
    {
        static_assert
        (
            detail::tlu::contains_v
            <
                event_type_list,
                Event
            >,
            "Given event type must be part of the type list given to `events()`"
        );
        impl_.process_event(evt);
    }

private:
    using event_type_list = typename std::decay_t<decltype(Conf)>::event_type_list;

    using event_impl_type = detail::tlu::apply_t
    <
        event_type_list,
        detail::machine_ref_event_impl
    >;

    event_impl_type impl_;
};

template<class... Events>
inline constexpr auto machine_ref_e_conf = machine_ref_conf{}
    .events<Events...>()
;

/**
@brief A convenient alias for @ref machine_ref that only takes a list of event
types
*/
template<class... Events>
using machine_ref_e = machine_ref<machine_ref_e_conf<Events...>>;

} //namespace

#endif

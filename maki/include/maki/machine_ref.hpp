//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_MACHINE_REF_HPP
#define MAKI_MACHINE_REF_HPP

#include "machine_ref_conf.hpp"
#include "machine_fwd.hpp"
#include "detail/tlu.hpp"

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
        template<class MachineDef>
        machine_ref_event_impl(machine<MachineDef>& mach):
            machine_ref_event_impl<Events...>{mach},
            pprocess_event_
            {
                [](void* const vpsm, const Event& event)
                {
                    using machine_t = machine<MachineDef>;
                    const auto psm = reinterpret_cast<machine_t*>(vpsm); //NOLINT
                    psm->process_event(event);
                }
            }
        {
        }

        using machine_ref_event_impl<Events...>::process_event;

        void process_event(const Event& event)
        {
            (*pprocess_event_)(get_vpsm(), event);
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
        template<class MachineDef>
        machine_ref_event_impl(machine<MachineDef>& mach):
            vpsm_(&mach)
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
machine_ref is a type-erasing container for a reference to a machine of any
type.
It exposes the process_event() member function of the held machine.
*/
template<class Def>
class machine_ref
{
public:
    static constexpr auto conf = Def::conf;

    template<class MachineDef>
    machine_ref(machine<MachineDef>& mach):
        impl_{mach}
    {
    }

    machine_ref(const machine_ref&) noexcept = default;
    machine_ref(machine_ref&&) noexcept = default;
    machine_ref& operator=(const machine_ref&) noexcept = default;
    machine_ref& operator=(machine_ref&&) noexcept = default;
    ~machine_ref() = default;

    template<class Event>
    void process_event(const Event& event)
    {
        static_assert
        (
            detail::tlu::contains_v
            <
                decltype(conf.event_types),
                Event
            >,
            "Given event type must be part of the 'events' option template argument list"
        );
        impl_.process_event(event);
    }

private:
    using event_type_list = decltype(conf.event_types);

    using event_impl_type = detail::tlu::apply_t
    <
        event_type_list,
        detail::machine_ref_event_impl
    >;

    event_impl_type impl_;
};

template<class... Events>
struct machine_ref_e_def
{
    static constexpr auto conf = machine_ref_conf_c
        .set_event_types<Events...>()
    ;
};

template<class... Events>
using machine_ref_e = machine_ref<machine_ref_e_def<Events...>>;

} //namespace

#endif

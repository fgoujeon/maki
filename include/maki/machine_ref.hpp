//Copyright Florian Goujeon 2021 - 2026.
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
#include "machine.hpp"
#include "detail/tlu/apply.hpp"
#include "detail/tlu/contains.hpp"
#include <type_traits>

namespace maki
{

namespace detail
{
    template<class AosVoid, class... Events>
    class any_machine_ref_impl;

    template<class AosVoid, class Event, class... Events>
    class any_machine_ref_impl<AosVoid, Event, Events...>: any_machine_ref_impl<AosVoid, Events...>
    {
    public:
        template<class MachineConfHolder>
        any_machine_ref_impl(machine<MachineConfHolder>& mach):
            any_machine_ref_impl<AosVoid, Events...>{mach},
            pprocess_event_{make_process_event_fn<MachineConfHolder>()}
        {
        }

        using any_machine_ref_impl<AosVoid, Events...>::process_event;

        void process_event(const Event& evt) const
        {
            (*pprocess_event_)(get_vpmach(), evt);
        }

#ifdef __cpp_impl_coroutine
        using any_machine_ref_impl<AosVoid, Events...>::async_process_event;

        AosVoid async_process_event(const Event& evt) const
        {
            co_await (*pprocess_event_)(get_vpmach(), evt);
        }
#endif

    protected:
        using any_machine_ref_impl<AosVoid, Events...>::get_vpmach;

    private:
        template<class MachineConfHolder>
        static constexpr auto make_process_event_fn()
        {
#ifdef __cpp_impl_coroutine
            if constexpr(std::is_void_v<AosVoid>)
            {
#endif
                return [](void* const vpmach, const Event& evt)
                {
                    using machine_t = machine<MachineConfHolder>;
                    const auto psm = reinterpret_cast<machine_t*>(vpmach); //NOLINT
                    psm->process_event(evt);
                };
#ifdef __cpp_impl_coroutine
            }
            else
            {
                return [](void* const vpmach, const Event& evt) -> AosVoid
                {
                    using machine_t = machine<MachineConfHolder>;
                    const auto psm = reinterpret_cast<machine_t*>(vpmach); //NOLINT
                    co_await psm->async_process_event(evt);
                };
            }
#endif
        }

        AosVoid(*pprocess_event_)(void*, const Event&) = nullptr;
    };

    template<class AosVoid>
    class any_machine_ref_impl<AosVoid>
    {
    public:
        template<class MachineConfHolder>
        any_machine_ref_impl(machine<MachineConfHolder>& mach):
            vpmach_(&mach)
        {
        }

        void process_event() const
        {
        }

#ifdef __cpp_impl_coroutine
        AosVoid async_process_event() const
        {
        }
#endif

    protected:
        [[nodiscard]] void* get_vpmach() const
        {
            return vpmach_;
        }

    private:
        void* vpmach_ = nullptr; //Pointer to `machine<...>`
    };

    template<class AosVoid>
    struct any_machine_ref_impl_holder
    {
        template<class... Events>
        using type = any_machine_ref_impl<AosVoid, Events...>;
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
    using conf_type = std::decay_t<decltype(Conf)>;

#if !MAKI_DETAIL_DOXYGEN
    static constexpr auto sync = std::is_void_v<typename conf_type::awaitable_template_holder>;
#endif

#ifdef __cpp_impl_coroutine
#if MAKI_DETAIL_DOXYGEN
    template<class T>
    using awaitable_type = IMPLEMENTATION_DETAIL;
#else
    struct sync_awaitable_type_holder
    {
        template<class T>
        using type = T;
    };

    struct async_awaitable_type_holder
    {
        template<class T>
        using type = typename conf_type::awaitable_template_holder::template type<T>;
    };

    template<class T>
    using awaitable_type = std::conditional_t
    <
        sync,
        sync_awaitable_type_holder,
        async_awaitable_type_holder
    >::template type<T>;
#endif
#endif

    template<class MachineConfHolder>
    machine_ref(machine<MachineConfHolder>& mach):
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
        static_assert(sync, "Only available in synchronous mode");
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

#ifdef __cpp_impl_coroutine
    template<class Event>
    awaitable_type<void> async_process_event(const Event& evt) const
    {
        static_assert(!sync, "Only available in asynchronous mode");
        static_assert
        (
            detail::tlu::contains_v
            <
                event_type_list,
                Event
            >,
            "Given event type must be part of the type list given to `events()`"
        );
        co_await impl_.async_process_event(evt);
    }
#endif

private:
    using event_type_list = typename conf_type::event_type_list;

#ifndef __cpp_impl_coroutine
    template<class T>
    using awaitable_type = T;
#endif

    using impl_type = detail::tlu::apply_t
    <
        event_type_list,
        detail::any_machine_ref_impl_holder<awaitable_type<void>>::template type
    >;

    impl_type impl_;
};


/*
machine_ref_e
*/

template<class... Events>
inline constexpr auto machine_ref_e_conf = machine_ref_conf{}
    .events<Events...>()
;

/**
@relates machine_ref
@brief A convenient alias for @ref machine_ref that only takes a list of event
types
*/
template<class... Events>
using machine_ref_e = machine_ref<machine_ref_e_conf<Events...>>;


/*
async_machine_ref_e
*/

template<template<class> class AwaitableTemplate, class... Events>
inline constexpr auto any_async_machine_ref_e_conf = machine_ref_conf{}
    .async<AwaitableTemplate>()
    .template events<Events...>()
;

template<template<class> class AwaitableTemplate, class... Events>
using any_async_machine_ref_e = machine_ref
<
    any_async_machine_ref_e_conf
    <
        AwaitableTemplate,
        Events...
    >
>;

} //namespace

#endif

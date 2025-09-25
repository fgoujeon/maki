//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_ANY_EVENT_REF_HPP
#define MAKI_DETAIL_ANY_EVENT_REF_HPP

#include "../event.hpp"
#include "../event_set.hpp"
#include <any>
#include <cassert>

namespace maki::detail
{

class any_event_ref
{
public:
    template<class T>
    any_event_ref(const T& event):
        any_(&event)
    {
    }

    any_event_ref(const any_event_ref&) = default;

    any_event_ref(any_event_ref&&) = delete;

    any_event_ref& operator=(const any_event_ref&) = default;

    any_event_ref& operator=(any_event_ref&&) = delete;

    template<class T>
    const T& get() const
    {
        return *std::any_cast<const T*>(any_);
    }

    template<class T>
    const T* get_if() const
    {
        const auto ppevent = std::any_cast<const T*>(&any_);
        if (!ppevent)
        {
            return nullptr;
        }
        return *ppevent;
    }

private:
    std::any any_;
};

template<class... Ts>
struct any_event_ref_visit_impl_list;

template<class T, class... Ts>
struct any_event_ref_visit_impl_list<T, Ts...>
{
    template<class F>
    static auto call(const F& func, const any_event_ref& event)
    {
        if(const auto pevent = event.template get_if<T>())
        {
            return func(*pevent);
        }
        else
        {
            return any_event_ref_visit_impl_list<Ts...>::call(func, event);
        }
    }
};

template<>
struct any_event_ref_visit_impl_list<>
{
    template<class F>
    static auto call(const F& func, const any_event_ref& event)
    {
        assert(false);
        return func(event);
    }
};

template<const auto& EventTypeHint, class F>
auto any_event_ref_visit(const F& func, const any_event_ref& event)
{
    using event_type_hint_type = std::decay_t<decltype(EventTypeHint)>;

    if constexpr(is_event_v<event_type_hint_type>)
    {
        return func(event.template get<typename event_type_hint_type::type>());
    }
    else if constexpr(is_event_set_v<event_type_hint_type>)
    {
        return tlu::apply_t
        <
            impl_of_t<event_type_hint_type>,
            any_event_ref_visit_impl_list
        >::call(func, event);
    }
    else
    {
        static_assert(false);
    }
}

template<const auto& /*EventTypeHint*/, class F, class Event>
auto any_event_ref_visit(const F& func, const Event& event)
{
    return func(event);
}

} //namespace

#endif

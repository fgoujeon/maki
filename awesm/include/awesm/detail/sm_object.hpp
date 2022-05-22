//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_OBJECT_HPP
#define AWESM_DETAIL_SM_OBJECT_HPP

#include <type_traits>
#include <optional>

/*
SM objects are states, actions and guards.
*/

namespace awesm::detail
{

namespace sm_object_detail
{
    template<class T, class... Args>
    constexpr auto is_brace_constructible_impl(int) -> decltype(T{std::declval<Args>()...}, bool())
    {
        return true;
    }

    template<class T, class... Args>
    constexpr bool is_brace_constructible_impl(long)
    {
        return false;
    }

    template<class T, class... Args>
    constexpr bool is_brace_constructible =
        is_brace_constructible_impl<T, Args...>(0)
    ;

    template<class T>
    static inline char storage[sizeof(T)];

    template<class T>
    static inline T* pointer;
}

/*
All SM objects can be constructed with one of these statements:
    auto obj = object_type{context, sm};
    auto obj = object_type{context};
    auto obj = object_type{};
*/

template<class T, class Context, class Sm>
void make_sm_object
(
    Context& ctx,
    Sm& machine,
    std::enable_if_t
    <
        sm_object_detail::is_brace_constructible<T, Context&, Sm&>
    >* = nullptr
)
{
    sm_object_detail::pointer<T> = new(sm_object_detail::storage<T>) T{ctx, machine};
}

template<class T, class Context, class Sm>
void make_sm_object
(
    Context& ctx,
    Sm&,
    std::enable_if_t
    <
        sm_object_detail::is_brace_constructible<T, Context&>
    >* = nullptr
)
{
    sm_object_detail::pointer<T> = new(sm_object_detail::storage<T>) T{ctx};
}

template<class T, class Context, class Sm>
void make_sm_object
(
    Context& /*ctx*/,
    Sm&,
    std::enable_if_t<std::is_default_constructible_v<T>>* = nullptr
)
{
    sm_object_detail::pointer<T> = new(sm_object_detail::storage<T>) T{};
}

template<class T, class Context, class Sm>
void make_sm_object_if_null(Context& ctx, Sm& machine)
{
    if(sm_object_detail::pointer<T> == nullptr)
    {
        make_sm_object<T>(ctx, machine);
    }
}

template<class T>
void unmake_sm_object()
{
    sm_object_detail::pointer<T>->~T();
    sm_object_detail::pointer<T> = nullptr;
}

template<class T>
void unmake_sm_object_if_not_null()
{
    if(sm_object_detail::pointer<T> != nullptr)
    {
        unmake_sm_object<T>();
    }
}

template<class T>
T& get_sm_object()
{
    return *sm_object_detail::pointer<T>;
}

} //namespace

#endif

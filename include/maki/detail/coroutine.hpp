//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_COROUTINE_HPP
#define MAKI_DETAIL_COROUTINE_HPP

#ifdef __cpp_impl_coroutine
#include <coroutine> //NOLINT misc-include-cleaner
#endif

#include <utility>

namespace maki::detail
{

#ifdef __cpp_impl_coroutine

template<typename Awaitable, typename Promise = void>
concept awaitable_type = requires (Awaitable aw, std::coroutine_handle<Promise> h)
{
    {aw.await_ready()} -> std::convertible_to<bool>;
    {aw.await_suspend(h)};
    {aw.await_resume()};
};

template<typename Awaitable, typename Promise = void>
concept awaitable =
    awaitable_type<Awaitable, Promise> ||
    requires(Awaitable && aw)
    {
        {std::forward<Awaitable>(aw).operator co_await()} -> awaitable_type<Promise>;
    } ||
    requires(Awaitable && aw)
    {
        {operator co_await(std::forward<Awaitable>(aw))} -> awaitable_type<Promise>;
    }
;

#else

template<typename Awaitable, typename Promise = void>
inline constexpr bool awaitable = false;

#endif

} //namespace

#endif

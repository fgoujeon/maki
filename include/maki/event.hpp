//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_EVENT_HPP
#define MAKI_EVENT_HPP

#include <type_traits>

namespace maki
{

/**
@brief Represents an event type
*/
template<class T>
struct event_t
{
    /**
    @brief An alias for the given type
    */
    using type = T;
};

/**
@brief Returns whether T and U are the same type
*/
template<class T, class U>
constexpr bool operator==(const event_t<T> /*lhs*/, const event_t<U> /*rhs*/)
{
    return std::is_same_v<T, U>;
}

/**
@brief A convenient variable template for `maki::event_t`
*/
template<class T>
constexpr auto event = event_t<T>{};

namespace detail
{
    template<class T>
    struct is_event
    {
        static constexpr auto value = false;
    };

    template<class T>
    struct is_event<event_t<T>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr bool is_event_v = is_event<T>::value;
}

} //namespace

#endif

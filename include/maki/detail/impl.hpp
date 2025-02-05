//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_IMPL_OF_HPP
#define MAKI_DETAIL_IMPL_OF_HPP

/*
Uniform way to access a private `impl_` member variable.
*/

#define MAKI_DETAIL_FRIENDLY_IMPL \
    template<class T> \
    friend struct detail::impl_of_t_helper; \
 \
    template<class T> \
    friend constexpr auto& detail::impl_of(T&); \
 \
    template<class T> \
    friend constexpr const auto& detail::impl_of(const T&); \
 \
    impl_type impl_;

namespace maki::detail
{

template<class T>
struct impl_of_t_helper
{
    using type = typename T::impl_type;
};

template<class T>
using impl_of_t = typename impl_of_t_helper<T>::type;

template<class T>
constexpr auto& impl_of(T& obj)
{
    return obj.impl_;
}

template<class T>
constexpr const auto& impl_of(const T& obj)
{
    return obj.impl_;
}

} //namespace

#endif

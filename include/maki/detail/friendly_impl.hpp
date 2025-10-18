//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_FRIENDLY_IMPL_HPP
#define MAKI_DETAIL_FRIENDLY_IMPL_HPP

/*
Numerous public classes of this library follow this recurring pattern:

    template<class Impl>
    class some_class
    {
    public:
        // ...

    private:
        MAKI_DETAIL_FRIENDLY_IMPL

        using impl_type = Impl;

        impl_type impl_;
    };

where `MAKI_DETAIL_FRIENDLY_IMPL` expands to a set of `friend` declarations
giving internals of the library access to the private parts of the class.

This file defines both the `MAKI_DETAIL_FRIENDLY_IMPL` and the types and
functions to access `impl_type` and `impl_`.
*/

#define MAKI_DETAIL_FRIENDLY_IMPL \
    template<class T> \
    friend struct detail::impl_of_t_helper; \
 \
    template<class T> \
    friend constexpr auto& detail::impl_of(T&); \
 \
    template<class T> \
    friend constexpr const auto& detail::impl_of(const T&);

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

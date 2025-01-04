//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_OVERLOAD_PRIORITY_HPP
#define MAKI_DETAIL_OVERLOAD_PRIORITY_HPP

namespace maki::detail::overload_priority
{

/*
Avoids ambiguous function call errors in cases where the existence of an
overload is conditional.

Example usage:

    template<class T>
    void f(overload_priority::low)
    {
        //...
    }

    template<class T, std::enable_if_t<some_condition<T>, bool> = true>
    void f(overload_priority::high)
    {
        //...
    }

    int main()
    {
        f<some_type>(overload_priority::probe);
    }

If some_condition<some_type> is false f(overload_priority::low) is called.
Otherwise, f(overload_priority::high) is called.
*/

using low = short;
using medium = int;
using high = char;

inline constexpr auto probe = ' ';

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MAYBE_BOOL_UTIL_HPP
#define MAKI_DETAIL_MAYBE_BOOL_UTIL_HPP

/*
Utility functions for parameter packs that are either [] or [bool].

Example:
    template<class... MaybeBool>
    void f(MaybeBool&... maybe_bool)
    {
        maybe_bool_util::set_to_true(maybe_bool...);
    }
*/

namespace maki::detail::maybe_bool_util
{

inline void set_to_true(bool& value)
{
    value = true;
}

inline constexpr void set_to_true()
{
    //nothing
}

} //namespace

#endif

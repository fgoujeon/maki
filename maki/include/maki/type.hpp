//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_TYPE_HPP
#define MAKI_TYPE_HPP

namespace maki
{

//A type holder
template<class T>
struct type_t
{
    using type = T;
};

template<class T>
constexpr auto type_c = type_t<T>{};

template<class T, class U>
constexpr bool operator==(const type_t<T>, const type_t<U>)
{
    return std::is_same_v<T, U>;
}

} //namespace

#endif

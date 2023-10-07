//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_TYPE_LIST_HPP
#define MAKI_TYPE_LIST_HPP

namespace maki
{

template<class... Ts>
struct type_list{};

template<class... Ts>
inline constexpr auto type_list_c = type_list<Ts...>{};

} //namespace

#endif
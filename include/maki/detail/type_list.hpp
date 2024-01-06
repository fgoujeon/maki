//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::type_list struct template
*/

#ifndef MAKI_DETAIL_TYPE_LIST_HPP
#define MAKI_DETAIL_TYPE_LIST_HPP

namespace maki::detail
{

/**
@brief A type list holder
*/
template<class... Ts>
struct type_list{};

/**
@brief A convenient variable template for @ref type_list
*/
template<class... Ts>
inline constexpr auto type_list_c = type_list<Ts...>{};

} //namespace

#endif

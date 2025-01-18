//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::detail::type_list_t struct template
*/

#ifndef MAKI_DETAIL_TYPE_LIST_HPP
#define MAKI_DETAIL_TYPE_LIST_HPP

namespace maki::detail
{

/**
@brief A type list holder
*/
template<class... Ts>
struct type_list_t{};

/**
@brief A convenient variable template for @ref type_list_t
*/
template<class... Ts>
inline constexpr auto type_list = type_list_t<Ts...>{};

} //namespace

#endif

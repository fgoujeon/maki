//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_ROOT_TAG_HPP
#define MAKI_DETAIL_ROOT_TAG_HPP

namespace maki::detail
{

struct root_tag_t{};
inline constexpr auto root_tag = root_tag_t{};

struct non_root_tag_t{};
inline constexpr auto non_root_tag = non_root_tag_t{};

} //namespace

#endif

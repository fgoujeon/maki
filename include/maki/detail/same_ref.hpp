//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SAME_REF_HPP
#define MAKI_DETAIL_SAME_REF_HPP

namespace maki::detail
{

template<class Rhs, class Lhs>
constexpr bool same_ref(const Rhs& rhs, const Lhs& lhs)
{
    return static_cast<const void*>(&rhs) == &lhs;
}

} //namespace

#endif

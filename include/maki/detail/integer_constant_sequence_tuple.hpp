//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_INTEGER_CONSTANT_SEQUENCE_TUPLE_HPP
#define MAKI_DETAIL_INTEGER_CONSTANT_SEQUENCE_TUPLE_HPP

#include "constant.hpp"
#include "tuple.hpp"
#include <utility>
#include <cstdlib>

namespace maki::detail
{

namespace integer_constant_sequence_tuple_detail
{
    template<class T, std::size_t I>
    struct make_helper
    {
        static constexpr auto seq = make_helper<T, I - 1>::seq.append(std::integral_constant<T, I - 1>{});
    };

    template<class T>
    struct make_helper<T, 0>
    {
        static constexpr auto seq = tuple{};
    };
}

template<class T, std::size_t Size>
constexpr auto integer_constant_sequence_tuple =
    integer_constant_sequence_tuple_detail::make_helper<T, Size>::seq
;

} //namespace

#endif

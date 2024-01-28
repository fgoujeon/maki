//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_APPLY_INDEX_SEQUENCE_HPP
#define MAKI_DETAIL_TLU_APPLY_INDEX_SEQUENCE_HPP

#include "size.hpp"

namespace maki::detail::tlu
{

/*
apply_index_sequence_t<TList, Target> is an alias of Target<Indexes...>, where
Indexes are the indexes of TList.
*/

template<class TList, template<int...> class Target, class IndexSequence>
struct apply_index_sequence_impl;

template<class TList, template<int...> class Target, int... Indexes>
struct apply_index_sequence_impl<TList, Target, std::integer_sequence<int, Indexes...>>
{
    using type = Target<Indexes...>;
};

template<class TList, template<int...> class Target>
using apply_index_sequence_t = typename apply_index_sequence_impl<TList, Target, std::make_integer_sequence<int, size_v<TList>>>::type;

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_MOLD_STORAGE_HPP
#define MAKI_DETAIL_STATE_MOLD_STORAGE_HPP

#include "state_set_fwd.hpp"

namespace maki::detail
{

template<class T>
struct state_mold_storage
{
    using type = const T&;
};

template<class StateSetImpl>
struct state_mold_storage<state_set<StateSetImpl>>
{
    using type = state_set<StateSetImpl>;
};

template<class T>
using state_mold_storage_t = typename state_mold_storage<T>::type;

} //namespace

#endif

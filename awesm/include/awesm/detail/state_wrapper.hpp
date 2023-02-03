//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_WRAPPER_HPP
#define AWESM_DETAIL_STATE_WRAPPER_HPP

#include "../state_conf.hpp"
#include "../composite_state_conf.hpp"
#include "../null.hpp"

namespace awesm::detail
{

template<class RegionPath, class WrappedState>
class composite_state_wrapper;

template<class RegionPath, class State, bool IsCompositeState>
struct state_wrapper_helper;

template<class RegionPath, class State>
struct state_wrapper_helper<RegionPath, State, false>
{
    using type = State;
};

template<class RegionPath, class State>
struct state_wrapper_helper<RegionPath, State, true>
{
    using type = composite_state_wrapper<RegionPath, State>;
};

template<class RegionPath, class State>
using state_wrapper_t = typename state_wrapper_helper
<
    RegionPath,
    State,
    State::conf_type::is_composite
>::type;

} //namespace

#endif

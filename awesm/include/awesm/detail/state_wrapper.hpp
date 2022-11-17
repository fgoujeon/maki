//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_WRAPPER_HPP
#define AWESM_DETAIL_STATE_WRAPPER_HPP

#include "../null.hpp"

namespace awesm
{

template<class TransitionTable, class... Options>
struct composite_state_conf;

} //namespace

namespace awesm::detail
{

template<class Sm, class RegionPath, class WrappedState>
class composite_state_wrapper;

template<class T>
struct is_composite_state_conf
{
    static constexpr auto value = false;
};

template<class TransitionTable, class... Options>
struct is_composite_state_conf<composite_state_conf<TransitionTable, Options...>>
{
    static constexpr auto value = true;
};

template<class T>
struct is_composite_state
{
    static constexpr auto value = is_composite_state_conf<typename T::conf_t>::value;
};

template<>
struct is_composite_state<null>
{
    static constexpr auto value = false;
};

template<class T>
constexpr auto is_composite_state_v = is_composite_state<T>::value;

template<class Sm, class RegionPath, class State, bool IsCompositeState>
struct state_wrapper_helper;

template<class Sm, class RegionPath, class State>
struct state_wrapper_helper<Sm, RegionPath, State, false>
{
    using type = State;
};

template<class Sm, class RegionPath, class State>
struct state_wrapper_helper<Sm, RegionPath, State, true>
{
    using type = composite_state_wrapper<Sm, RegionPath, State>;
};

template<class Sm, class RegionPath, class State>
using state_wrapper_t = typename state_wrapper_helper<Sm, RegionPath, State, is_composite_state_v<State>>::type;

} //namespace

#endif

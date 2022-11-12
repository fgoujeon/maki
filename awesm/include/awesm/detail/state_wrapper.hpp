//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_WRAPPER_HPP
#define AWESM_DETAIL_STATE_WRAPPER_HPP

namespace awesm
{

template<class TransitionTable, class... Options>
struct composite_state_conf;

} //namespace

namespace awesm::detail
{

template<class WrappedState>
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
    static constexpr auto value = is_composite_state_conf<typename T::conf>::value;
};

template<>
struct is_composite_state<void>
{
    static constexpr auto value = false;
};

template<class T>
constexpr auto is_composite_state_v = is_composite_state<T>::value;

template<class State>
constexpr auto get_state_wrapper()
{
    if constexpr(is_composite_state_v<State>)
    {
        return static_cast<composite_state_wrapper<State>*>(nullptr);
    }
    else
    {
        return static_cast<State*>(nullptr);
    }
}

template<class State, bool IsCompositeState>
struct state_wrapper_helper;

template<class State>
struct state_wrapper_helper<State, false>
{
    using type = State;
};

template<class State>
struct state_wrapper_helper<State, true>
{
    using type = composite_state_wrapper<State>;
};

template<class State>
using state_wrapper_t = typename state_wrapper_helper<State, is_composite_state_v<State>>::type;

} //namespace

#endif

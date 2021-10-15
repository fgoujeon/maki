//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_without_event.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_CALL_STATE_MEMBER_HPP
#define FGFSM_DETAIL_CALL_STATE_MEMBER_HPP

#include "../event.hpp"

namespace fgfsm::detail
{

/*
call_on_xxx(state, event) calls either (in this order of priority):
- state.on_xxx(event);
- state.on_xxx() if the above function doesn't exist;
- nothing, if the above functions don't exist.
*/

#define FGFSM_DETAIL_CALL_STATE_MEMBER(MEMBER) \
    template<class State> \
    auto call_##MEMBER##_with_event(State& state, const event& evt, int) -> decltype(state.MEMBER(evt), bool()) \
    { \
        state.MEMBER(evt); \
        return true; \
    } \
 \
    template<class State> \
    bool call_##MEMBER##_with_event(State&, const event&, long) \
    { \
        return false; \
    } \
 \
    template<class State> \
    auto call_##MEMBER##_without_event(State& state, int) -> decltype(state.MEMBER(), bool()) \
    { \
        state.MEMBER(); \
        return true; \
    } \
 \
    template<class State> \
    bool call_##MEMBER##_without_event(State&, long) \
    { \
        return false; \
    } \
 \
    template<class State> \
    void call_##MEMBER(State& state, const event& evt) \
    { \
        call_##MEMBER##_with_event(state, evt, 0) || \
        call_##MEMBER##_without_event(state, 0); \
    }

FGFSM_DETAIL_CALL_STATE_MEMBER(on_entry)
FGFSM_DETAIL_CALL_STATE_MEMBER(on_event)
FGFSM_DETAIL_CALL_STATE_MEMBER(on_exit)

#undef FGFSM_DETAIL_CALL_STATE_MEMBER

} //namespace

#endif

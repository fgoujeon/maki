//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::action class template
*/

#ifndef MAKI_ACTION_HPP
#define MAKI_ACTION_HPP

#include "detail/event_action.hpp"

namespace maki
{

/*
v means void
m means machine
c means context
e means event
*/
enum class action_signature: char
{
#define MAKI_DETAIL_X(name) name, /*NOLINT(cppcoreguidelines-macro-usage)*/
    MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X
};

template<class Callable>
struct action
{
    action_signature signature;
    Callable callable;
};

template<class Callable>
action(action_signature sig, const Callable&) -> action<Callable>;

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class Callable> \
    constexpr auto action_##name(const Callable& callable) \
    { \
        return action{action_signature::name, callable}; \
    }
MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

} //namespace

#endif

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

template<action_signature Sig, class Callable>
struct action
{
    static constexpr action_signature signature = Sig;
    Callable callable;
};

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    template<class Callable> \
    constexpr action<action_signature::name, Callable> action_##name(const Callable& callable) \
    { \
        return {callable}; \
    }
MAKI_DETAIL_EVENT_ACTION_SIGNATURES
#undef MAKI_DETAIL_X

} //namespace

#endif

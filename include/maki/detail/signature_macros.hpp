//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIGNATURES_HPP
#define MAKI_DETAIL_SIGNATURES_HPP

/*
a means list of arguments given to machine constructor
v means void
c means context
m means machine
e means event
*/

#define MAKI_DETAIL_ACTION_SIGNATURES \
    MAKI_DETAIL_X(v) \
    MAKI_DETAIL_X(c) \
    MAKI_DETAIL_X(cm) \
    MAKI_DETAIL_X(cme) \
    MAKI_DETAIL_X(ce) \
    MAKI_DETAIL_X(m) \
    MAKI_DETAIL_X(me) \
    MAKI_DETAIL_X(e)

#define MAKI_DETAIL_GUARD_SIGNATURES \
    MAKI_DETAIL_ACTION_SIGNATURES

#define MAKI_DETAIL_MACHINE_CONTEXT_CONSTRUCTOR_SIGNATURES \
    MAKI_DETAIL_X(a) \
    MAKI_DETAIL_X(am)

#define MAKI_DETAIL_STATE_CONTEXT_CONSTRUCTOR_SIGNATURES \
    MAKI_DETAIL_X(v) \
    MAKI_DETAIL_X(c) \
    MAKI_DETAIL_X(cm) \
    MAKI_DETAIL_X(m)

#define MAKI_DETAIL_CONTEXT_CONSTRUCTOR_SIGNATURES \
    MAKI_DETAIL_MACHINE_CONTEXT_CONSTRUCTOR_SIGNATURES \
    MAKI_DETAIL_STATE_CONTEXT_CONSTRUCTOR_SIGNATURES

#endif

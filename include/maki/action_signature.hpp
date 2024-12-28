//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_ACTION_SIGNATURE_HPP
#define MAKI_ACTION_SIGNATURE_HPP

#include "detail/signature_macros.hpp"

namespace maki
{

/**
@brief The set of arguments taken by an action callable.

Meaning of every letter:

- `v`: void
- `m`: machine
- `c`: context
- `e`: event
*/
enum class action_signature: char
{
#define MAKI_DETAIL_X(name) name, /*NOLINT(cppcoreguidelines-macro-usage)*/
    MAKI_DETAIL_ACTION_SIGNATURES
#undef MAKI_DETAIL_X
};

} //namespace

#endif
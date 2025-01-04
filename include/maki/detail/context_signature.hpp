//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_SIGNATURE_HPP
#define MAKI_DETAIL_CONTEXT_SIGNATURE_HPP

#include "signature_macros.hpp"

namespace maki::detail
{

enum class context_signature: char
{
#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    name,
    MAKI_DETAIL_CONTEXT_CONSTRUCTOR_SIGNATURES
#undef MAKI_DETAIL_X
};

} //namespace

#endif

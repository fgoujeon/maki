//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_CONTEXT_SIGNATURE_HPP
#define MAKI_DETAIL_CONTEXT_SIGNATURE_HPP

/*
a means list of arguments given to machine constructor
v means void
c means context
m means machine
*/

#define MAKI_DETAIL_CONTEXT_SIGNATURES_FOR_MACHINE \
    MAKI_DETAIL_X(a) \
    MAKI_DETAIL_X(am)

#define MAKI_DETAIL_CONTEXT_SIGNATURES_FOR_STATE \
    MAKI_DETAIL_X(v) \
    MAKI_DETAIL_X(c) \
    MAKI_DETAIL_X(cm) \
    MAKI_DETAIL_X(m)

namespace maki::detail
{

struct context_signature_auto_tag_t{};
inline constexpr auto context_signature_auto_tag =
    context_signature_auto_tag_t{}
;

#define MAKI_DETAIL_X(name) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    struct context_signature_##name##_tag_t{};

MAKI_DETAIL_CONTEXT_SIGNATURES_FOR_MACHINE
MAKI_DETAIL_CONTEXT_SIGNATURES_FOR_STATE

#undef MAKI_DETAIL_X

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_NOINLINE_HPP
#define MAKI_DETAIL_NOINLINE_HPP

#ifdef _MSC_VER
#   define MAKI_NOINLINE __declspec(noinline)
#else
#   define MAKI_NOINLINE __attribute__((noinline))
#endif

#endif

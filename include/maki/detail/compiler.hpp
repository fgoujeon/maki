//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_COMPILER_HPP
#define MAKI_DETAIL_COMPILER_HPP

/*
`MAKI_DETAIL_COMPILER_GCC`
Note that GCC is not the only compiler to define `__GNUC__`, so we need extra
tests.
*/
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#define MAKI_DETAIL_COMPILER_GCC 1
#else
#define MAKI_DETAIL_COMPILER_GCC 0
#endif

#endif

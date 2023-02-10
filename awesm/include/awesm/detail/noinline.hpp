//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_NOINLINE_HPP
#define AWESM_DETAIL_NOINLINE_HPP

#ifdef _MSC_VER
#   define AWESM_NOINLINE __declspec(noinline)
#else
#   define AWESM_NOINLINE __attribute__((noinline))
#endif

#endif

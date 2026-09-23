// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_PP_ENUM_HPP
#define MAKI_DETAIL_PP_ENUM_HPP

#define MAKI_DETAIL_PP_ENUM_1(DO) DO(0)
#define MAKI_DETAIL_PP_ENUM_2(DO) MAKI_DETAIL_PP_ENUM_1(DO), DO(1)
#define MAKI_DETAIL_PP_ENUM_3(DO) MAKI_DETAIL_PP_ENUM_2(DO), DO(2)
#define MAKI_DETAIL_PP_ENUM_4(DO) MAKI_DETAIL_PP_ENUM_3(DO), DO(3)
#define MAKI_DETAIL_PP_ENUM_5(DO) MAKI_DETAIL_PP_ENUM_4(DO), DO(4)
#define MAKI_DETAIL_PP_ENUM_6(DO) MAKI_DETAIL_PP_ENUM_5(DO), DO(5)
#define MAKI_DETAIL_PP_ENUM_7(DO) MAKI_DETAIL_PP_ENUM_6(DO), DO(6)
#define MAKI_DETAIL_PP_ENUM_8(DO) MAKI_DETAIL_PP_ENUM_7(DO), DO(7)
#define MAKI_DETAIL_PP_ENUM_9(DO) MAKI_DETAIL_PP_ENUM_8(DO), DO(8)
#define MAKI_DETAIL_PP_ENUM_10(DO) MAKI_DETAIL_PP_ENUM_9(DO), DO(9)

#endif

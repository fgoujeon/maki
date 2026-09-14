//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#define AOS(name) name
#define AOS_ASYNC 0
#define AOS_ASYNC_OPTS
#define AOS_CALL
#define AOS_RETURN return

#define AOS_VOID void

#define AOS_TEST(name) \
    TEST_CASE(#name)

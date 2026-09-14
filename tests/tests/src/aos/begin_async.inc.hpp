//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#define AOS(name) async_##name
#define AOS_ASYNC 1
#define AOS_ASYNC_OPTS .async<boost::cobalt::promise>()
#define AOS_CALL co_await
#define AOS_RETURN co_return

#define AOS_TEST(name) \
    boost::cobalt::task<void> test(); \
 \
    TEST_CASE("async_" #name) \
    { \
        boost::cobalt::run(test()); \
    } \
 \
    boost::cobalt::task<void> test()

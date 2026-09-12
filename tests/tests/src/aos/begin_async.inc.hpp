//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#define AOS(name) async_##name
#define AOS_ASYNC 1
#define AOS_ASYNC_OPTS .async<boost::cobalt::promise>()
#define AOS_CALL co_await
#define AOS_PROMISE_TYPE(type) boost::cobalt::promise<type>
#define AOS_TASK_TYPE(type) boost::cobalt::task<type>

#define AOS_TEST_CASE(name) \
    TEST_CASE("async_" #name) \
    { \
        boost::cobalt::run(async_##name##_ns::test()); \
    }

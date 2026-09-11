//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/*
AOS stands for "Asynchronous Or Synchronous".
This file defines macros for asynchronous code.
*/

#define MAKI_AOS_ASYNC 1
#define MAKI_AOS_NAME(name) async_##name
#define MAKI_AOS_CALL co_await
#define MAKI_AOS_RETURN co_return
#define MAKI_AOS_TYPE(type) awaitable_type<type>
#define MAKI_AOS_VOID detail::co_util::awaitable_t<typename std::decay_t<decltype(detail::impl_of(MachineConfHolder::value))>::awaitable_template_holder, void>
#define MAKI_AOS_BOOL detail::co_util::awaitable_t<typename std::decay_t<decltype(detail::impl_of(MachineConfHolder::value))>::awaitable_template_holder, bool>
#define MAKI_AOS_INT  detail::co_util::awaitable_t<typename std::decay_t<decltype(detail::impl_of(MachineConfHolder::value))>::awaitable_template_holder, int>

//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifdef MAKI_BUILD_TESTS_20

#include <maki.hpp>
#include "common.hpp"
#include <boost/cobalt.hpp>
#include <boost/asio/steady_timer.hpp>
#include <iostream>

namespace async_ns
{
    struct context
    {
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        constexpr auto off = maki::state_mold{};

        namespace on_ns
        {
            boost::cobalt::task<void> co_work(context& ctx);

            struct context
            {
                async_ns::context& parent;

                bool co_work_started = false;
                bool co_work_completed = false;

                std::optional<boost::cobalt::promise<void>> opt_co_work_promise;
            };

            boost::cobalt::promise<void> co_work(context& ctx)
            {
                try
                {
                    ctx.co_work_started = true;
                    auto timer = boost::asio::steady_timer
                    {
                        co_await boost::asio::this_coro::executor,
                        std::chrono::steady_clock::now() + std::chrono::seconds(5)
                    };
                    co_await timer.async_wait();
                    ctx.co_work_completed = true;
                }
                catch(...)
                {
                }
            }
        }

        constexpr auto on = maki::state_mold{}
            .context_c<on_ns::context>()
            .entry_action_c
            (
                [](on_ns::context& ctx)
                {
                    ctx.opt_co_work_promise = on_ns::co_work(ctx);
                }
            )
            .exit_action_c
            (
                [](on_ns::context& ctx) -> boost::cobalt::promise<void>
                {
                    ctx.opt_co_work_promise->cancel();
                    co_await *ctx.opt_co_work_promise;
                    ctx.opt_co_work_promise.reset();
                }
            )
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::ini,   states::off)
        (states::off, states::on,  maki::event<events::button_press>)
        (states::on,  states::off, maki::event<events::button_press>)
    ;

    struct machine_conf
    {
        static constexpr auto value = maki::machine_conf{}
            .transition_tables(transition_table)
            .context_a<context>()
            .async<boost::cobalt::promise>()
        ;
    };

    using machine_t = maki::async_machine<machine_conf>;

    boost::cobalt::task<void> co_test()
    {
        auto machine = machine_t{};

        co_await machine.start();
        CHECK(machine.running());
        CHECK(machine.is<states::off>());

        co_await machine.process_event(events::button_press{});
        CHECK(machine.is<states::on>());
        CHECK(machine.state<states::on>().context().co_work_started);
        CHECK(!machine.state<states::on>().context().co_work_completed);

        co_await machine.process_event(events::button_press{});
        CHECK(machine.is<states::off>());
    }
}

TEST_CASE("async")
{
    using namespace async_ns;
    boost::cobalt::run(co_test());
}

#endif

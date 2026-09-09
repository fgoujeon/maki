//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifdef __cpp_impl_coroutine

#include <maki.hpp>
#include "common.hpp"
#include <boost/cobalt.hpp>

namespace async_ns
{
    struct context
    {
        void boop()
        {
            i = 0;
        }

        int i = 0;
    };

    namespace events
    {
        struct button_press{};
    }

    namespace states
    {
        EMPTY_STATE(off)
        EMPTY_STATE(on)
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
        REQUIRE(machine.running());
        REQUIRE(machine.is<states::off>());

        co_await machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::on>());
        REQUIRE(machine.context().i == 1);

        co_await machine.process_event(events::button_press{});
        REQUIRE(machine.is<states::off>());
        REQUIRE(machine.context().i == 0);
    }

    void test()
    {
        auto ioc = boost::asio::io_context{};

        boost::cobalt::spawn
        (
            ioc,
            co_test(),
            [&ioc](const std::exception_ptr& eptr)
            {
                ioc.stop();
            }
        );

        ioc.run();
    }
}

TEST_CASE("async")
{
    using namespace async_ns;
    auto thread = std::jthread{&test};
}

#endif

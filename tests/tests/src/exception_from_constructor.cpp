//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"
#include <string>

namespace exception_from_constructor_ns
{
    struct context
    {
        int always_zero = 0;
        std::string out;
    };

    namespace states
    {
        constexpr auto running = maki::state_builder{}
            .entry_action_c([](context& ctx)
            {
                if(ctx.always_zero == 0) //We need this to avoid "unreachable code" warnings
                {
                    throw std::runtime_error{"error"};
                }
            })
        ;
    }

    constexpr auto transition_table = maki::transition_table{}
        (maki::init, states::running)
    ;

    constexpr auto machine_conf = maki::machine_conf{}
        .transition_tables(transition_table)
        .context_a<context>()
        .catch_mx([](auto& mach, const std::exception_ptr eptr)
        {
            try
            {
                std::rethrow_exception(eptr);
            }
            catch(const std::exception& ex)
            {
                mach.context().out += std::string{"Caught "} + ex.what();
            }
        })
    ;

    using sm_t = maki::machine<machine_conf>;
}

TEST_CASE("exception_from_constructor")
{
    using namespace exception_from_constructor_ns;

    auto machine = sm_t{};

    CHECK(machine.context().out == "Caught error");
}

//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct context
    {
    };

    struct int_data
    {
        int value = 0;
    };

    struct text_data
    {
        std::string value;
    };

    struct is_positive_int_1
    {
        bool check(const fgfsm::any_cref& event)
        {
            return fgfsm::visit_or_false
            (
                event,
                [](const int_data& event)
                {
                    return event.value > 0;
                }
            );
        }

        context& ctx;
    };

    bool is_positive_int_2_impl(context& ctx, const fgfsm::any_cref& event)
    {
        return fgfsm::visit_or_false
        (
            event,
            [](const int_data& event)
            {
                return event.value > 0;
            }
        );
    }
    using is_positive_int_2 = fgfsm::guard_fn<is_positive_int_2_impl>;

    bool is_positive_int_3_impl(context& ctx, const int_data& event)
    {
        return event.value > 0;
    }
    using is_positive_int_3 = fgfsm::guard_fn<is_positive_int_3_impl>;
}

TEST_CASE("guard_fn")
{
    auto ctx = context{};
    auto guard_1 = is_positive_int_1{ctx};
    auto guard_2 = is_positive_int_2{ctx};
    auto guard_3 = is_positive_int_3{ctx};

    {
        const auto value = int_data{1};
        REQUIRE(guard_1.check(value));
        REQUIRE(guard_2.check(value));
        REQUIRE(guard_3.check(value));
    }

    {
        const auto value = int_data{-1};
        REQUIRE(!guard_1.check(value));
        REQUIRE(!guard_2.check(value));
        REQUIRE(!guard_3.check(value));
    }

    {
        const auto value = text_data{"1"};
        REQUIRE(!guard_1.check(value));
        REQUIRE(!guard_2.check(value));
        REQUIRE(!guard_3.check(value));
    }
}

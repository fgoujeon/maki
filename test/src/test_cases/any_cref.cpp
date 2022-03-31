//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/any_cref.hpp>
#include <catch2/catch.hpp>
#include <string>

namespace
{
    struct int_data
    {
        int value = 0;
    };

    struct text_data
    {
        std::string value;
    };

    bool is_positive_int(const fgfsm::any_cref& obj)
    {
        return visit_or_false
        (
            obj,
            [](const int_data& obj)
            {
                return obj.value > 0;
            }
        );
    }
}

TEST_CASE("any_cref")
{
    auto str = std::string{};

    auto assign_str = [&](const fgfsm::any_cref& any)
    {
        visit
        (
            any,
            [&](const int_data& obj)
            {
                str = std::to_string(obj.value);
            },
            [&](const text_data& obj)
            {
                str = obj.value;
            }
        );
    };

    assign_str(int_data{42});
    REQUIRE(str == "42");

    assign_str(text_data{"abc"});
    REQUIRE(str == "abc");

    REQUIRE(is_positive_int(int_data{1}));
    REQUIRE(!is_positive_int(int_data{-1}));
    REQUIRE(!is_positive_int(text_data{"1"}));
}

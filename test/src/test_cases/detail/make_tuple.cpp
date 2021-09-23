//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/detail/make_tuple.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
    struct type0
    {
        int i;
        char c;
    };

    struct type1
    {
        int i;
        char c;
    };

    using type_tuple = std::tuple<type0, type1>;
}

TEST_CASE("detail::make_tuple")
{
    //const
    {
        const auto tuple = fgfsm::detail::make_tuple<type_tuple>(0, 'a');
        const auto& obj0 = std::get<type0>(tuple);
        const auto& obj1 = std::get<type1>(tuple);

        REQUIRE(obj0.i == 0);
        REQUIRE(obj1.i == 0);
        REQUIRE(obj0.c == 'a');
        REQUIRE(obj1.c == 'a');
    }

    //mutable
    {
        auto tuple = fgfsm::detail::make_tuple<type_tuple>(0, 'a');
        auto& obj0 = std::get<type0>(tuple);
        auto& obj1 = std::get<type1>(tuple);

        REQUIRE(obj0.i == 0);
        REQUIRE(obj1.i == 0);
        REQUIRE(obj0.c == 'a');
        REQUIRE(obj1.c == 'a');
    }
}

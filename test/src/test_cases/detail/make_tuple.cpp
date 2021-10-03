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
        int& data;
    };

    struct type1
    {
        int& data;
    };

    using type_tuple = std::tuple<type0, type1>;
}

TEST_CASE("detail::make_tuple")
{
    auto data = 42;
    auto tuple = fgfsm::detail::make_tuple<type_tuple>(data);
    auto& obj0 = std::get<type0>(tuple);
    auto& obj1 = std::get<type1>(tuple);

    REQUIRE(&obj0.data == &data);
    REQUIRE(&obj1.data == &data);
}

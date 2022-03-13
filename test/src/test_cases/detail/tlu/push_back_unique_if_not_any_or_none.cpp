//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/detail/tlu/push_back_unique_if_not_any_or_none.hpp>
#include <catch2/catch.hpp>
#include <tuple>

TEST_CASE("detail::tlu::push_back_unique_if_not_any_or_none")
{
    using fgfsm::detail::tlu::push_back_unique_if_not_any_or_none;

    using typelist = std::tuple<char, short, int>;
    using typelist2 = push_back_unique_if_not_any_or_none<typelist, long>;
    using typelist3 = push_back_unique_if_not_any_or_none<typelist, short>;
    using typelist4 = push_back_unique_if_not_any_or_none<typelist, fgfsm::any>;
    using typelist5 = push_back_unique_if_not_any_or_none<typelist, fgfsm::none>;

    using expected_typelist2 = std::tuple<char, short, int, long>;
    REQUIRE(std::is_same_v<typelist2, expected_typelist2>);

    using expected_typelist3 = typelist;
    REQUIRE(std::is_same_v<typelist3, expected_typelist3>);

    using expected_typelist4 = typelist;
    REQUIRE(std::is_same_v<typelist4, expected_typelist4>);

    using expected_typelist5 = typelist;
    REQUIRE(std::is_same_v<typelist5, expected_typelist5>);
}

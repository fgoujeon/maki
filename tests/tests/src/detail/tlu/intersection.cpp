//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/tlu/intersection.hpp>
#include "../../common.hpp"
#include <tuple>

TEST_CASE("detail::tlu::intersection")
{
    {
        using typelist = std::tuple<char, short, int>;
        using typelist2 = std::tuple<char, int>;
        using result_typelist = maki::detail::tlu::intersection_t<typelist, typelist2>;

        using expected_result_typelist = std::tuple<char, int>;
        REQUIRE(std::is_same_v<result_typelist, expected_result_typelist>);
    }

    {
        using typelist = std::tuple<>;
        using typelist2 = std::tuple<char, int>;
        using result_typelist = maki::detail::tlu::intersection_t<typelist, typelist2>;

        using expected_result_typelist = std::tuple<>;
        REQUIRE(std::is_same_v<result_typelist, expected_result_typelist>);
    }

    {
        using typelist = std::tuple<char, int>;
        using typelist2 = std::tuple<double, double*>;
        using result_typelist = maki::detail::tlu::intersection_t<typelist, typelist2>;

        using expected_result_typelist = std::tuple<>;
        REQUIRE(std::is_same_v<result_typelist, expected_result_typelist>);
    }
}

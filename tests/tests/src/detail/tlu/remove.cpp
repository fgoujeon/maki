//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/tlu/remove.hpp>
#include "../../common.hpp"
#include <tuple>

TEST_CASE("detail::tlu::remove")
{
    {
        using typelist = std::tuple<char, short, int>;
        using typelist2 = maki::detail::tlu::remove_t<typelist, char>;
        using expected_typelist2 = std::tuple<short, int>;
        REQUIRE(std::is_same_v<typelist2, expected_typelist2>);
    }

    {
        using typelist = std::tuple<>;
        using typelist2 = maki::detail::tlu::remove_t<typelist, char>;
        using expected_typelist2 = std::tuple<>;
        REQUIRE(std::is_same_v<typelist2, expected_typelist2>);
    }

    {
        using typelist = std::tuple<short, double>;
        using typelist2 = maki::detail::tlu::remove_t<typelist, int>;
        using expected_typelist2 = std::tuple<short, double>;
        REQUIRE(std::is_same_v<typelist2, expected_typelist2>);
    }
}

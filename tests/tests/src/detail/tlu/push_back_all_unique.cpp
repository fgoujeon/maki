//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/tlu/push_back_all_unique.hpp>
#include "../../common.hpp"
#include <tuple>

TEST_CASE("detail::tlu::push_back_all_unique")
{
    using typelist = std::tuple<char, short, int>;
    using typelist2 = std::tuple<double, int>;

    using typelist3 = maki::detail::tlu::push_back_all_unique_t<typelist, typelist2>;
    using expected_typelist3 = std::tuple<char, short, int, double>;
    REQUIRE(std::is_same_v<typelist3, expected_typelist3>);
}

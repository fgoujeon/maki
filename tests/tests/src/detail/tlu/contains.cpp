//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/tlu/contains.hpp>
#include "../../common.hpp"
#include <tuple>

TEST_CASE("detail::tlu::contains")
{
    using type_list = std::tuple<char, short, int, long>;
    REQUIRE(maki::detail::tlu::contains_v<type_list, int>);
    REQUIRE(!maki::detail::tlu::contains_v<type_list, double>);
}

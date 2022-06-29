//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/tlu/contains.hpp>
#include "../../common/catch.hpp"
#include <tuple>

TEST_CASE("detail::tlu::contains")
{
    using type_list = std::tuple<char, short, int, long>;
    REQUIRE(awesm::detail::tlu::contains<type_list, int>);
    REQUIRE(!awesm::detail::tlu::contains<type_list, double>);
}

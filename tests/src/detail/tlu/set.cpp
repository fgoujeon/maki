//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/tlu/set.hpp>
#include "../../common.hpp"
#include <tuple>

TEST_CASE("detail::tlu::set")
{
    using typelist = std::tuple<char, short, int>;
    using typelist2 = awesm::detail::tlu::set_t<typelist, 1, long>;

    using expected_typelist2 = std::tuple<char, long, int>;
    REQUIRE(std::is_same_v<typelist2, expected_typelist2>);
}

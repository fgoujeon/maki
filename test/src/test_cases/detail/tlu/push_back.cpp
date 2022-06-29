//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/tlu/push_back.hpp>
#include "../../common/catch.hpp"
#include <tuple>

TEST_CASE("detail::tlu::push_back")
{
    using typelist = std::tuple<char, short, int>;
    using typelist2 = awesm::detail::tlu::push_back<typelist, long>;

    using expected_typelist2 = std::tuple<char, short, int, long>;
    REQUIRE(std::is_same_v<typelist2, expected_typelist2>);
}

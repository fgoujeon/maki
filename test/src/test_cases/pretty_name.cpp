//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm.hpp>
#include "common/catch.hpp"

namespace pretty_name_ns
{
    struct test{};

    template<class T, class U>
    class templ{};
}

TEST_CASE("pretty_name")
{
    REQUIRE(awesm::get_pretty_name<pretty_name_ns::test>() == "test");
    REQUIRE
    (
        awesm::get_pretty_name<pretty_name_ns::templ<int, pretty_name_ns::test>>() ==
        "templ"
    );
}

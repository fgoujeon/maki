//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/sm_object_holder_tuple.hpp>
#include "../common/catch.hpp"

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

    using type_tuple = awesm::detail::sm_object_holder_tuple<type0, type1>;
}

TEST_CASE("detail::make_tuple")
{
    auto data = 42;
    auto tuple = type_tuple{data, data /*ignored*/};
    auto& obj0 = tuple.get(static_cast<type0*>(nullptr));
    auto& obj1 = tuple.get(static_cast<type1*>(nullptr));

    REQUIRE(&obj0.data == &data);
    REQUIRE(&obj1.data == &data);
}

//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/detail/for_each.hpp>
#include <fgfsm/detail/fsm_object_holder_tuple.hpp>
#include <catch2/catch.hpp>

namespace
{
    struct type0
    {
        int i = 0;
    };

    struct type1
    {
        int i = 1;
    };

    using type_tuple = fgfsm::detail::fsm_object_holder_tuple<type0, type1>;
}

TEST_CASE("detail::for_each")
{
    void* ignored = nullptr;
    auto tuple = type_tuple{ignored, ignored};

    fgfsm::detail::for_each
    (
        tuple,
        [](const auto& element)
        {
            using type = std::decay_t<decltype(element)>;

            if constexpr(std::is_same_v<type, type0>)
            {
                REQUIRE(element.i == 0);
            }

            if constexpr(std::is_same_v<type, type1>)
            {
                REQUIRE(element.i == 1);
            }
        }
    );
}

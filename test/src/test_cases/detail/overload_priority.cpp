//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include "../common.hpp"
#include <awesm/detail/overload_priority.hpp>

namespace
{
    template<int I>
    std::string_view f(awesm::detail::overload_priority::low)
    {
        return "low";
    }

    template<int I, std::enable_if_t<(I >= 1), bool> = true>
    std::string_view f(awesm::detail::overload_priority::medium)
    {
        return "medium";
    }

    template<int I, std::enable_if_t<(I >= 2), bool> = true>
    std::string_view f(awesm::detail::overload_priority::high)
    {
        return "high";
    }
}

TEST_CASE("detail::overload_priority")
{
    REQUIRE(f<0>(awesm::detail::overload_priority::probe) == "low");
    REQUIRE(f<1>(awesm::detail::overload_priority::probe) == "medium");
    REQUIRE(f<2>(awesm::detail::overload_priority::probe) == "high");
}

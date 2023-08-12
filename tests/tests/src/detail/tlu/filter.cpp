//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/tlu/filter.hpp>
#include "../../common.hpp"
#include <tuple>

namespace
{
    template<class T>
    struct is_char_or_int
    {
        static constexpr auto value = std::is_same_v<T, int> || std::is_same_v<T, char>;
    };
}

TEST_CASE("detail::tlu::filter")
{
    using type_list = std::tuple<char, short, int, long>;

    using filtered_type_list = maki::detail::tlu::filter_t<type_list, is_char_or_int>;
    using expected_filtered_type_list = std::tuple<char, int>;

    REQUIRE(std::is_same_v<filtered_type_list, expected_filtered_type_list>);
}

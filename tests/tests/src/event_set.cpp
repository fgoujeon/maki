//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

TEST_CASE("event_set_basic")
{
    {
        constexpr auto es = !maki::event<int>;
        REQUIRE(!es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(es.contains<char>());
    }

    {
        constexpr auto es = maki::event<int> || maki::event<double>;
        REQUIRE(es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(!es.contains<char>());
    }

    {
        constexpr auto es = !maki::event<int> || maki::event<double>;
        REQUIRE(!es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(es.contains<char>());
    }

    {
        constexpr auto es = (maki::event<int> || maki::event<char>) && (maki::event<int> || maki::event<double>);
        REQUIRE(es.contains<int>());
        REQUIRE(!es.contains<double>());
        REQUIRE(!es.contains<char>());
    }

    {
        constexpr auto es = !maki::event<char> || (maki::event<int> || maki::event<double>);
        REQUIRE(es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(!es.contains<char>());
    }

    {
        constexpr auto es = !(maki::event<char> || maki::event<int>) || (maki::event<int> || maki::event<double>);
        REQUIRE(es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(!es.contains<char>());
    }

    {
        constexpr auto es = !maki::event<int> || (maki::event<int> || maki::event<double>);
        REQUIRE(es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(es.contains<char>());
    }

    {
        constexpr auto es = !maki::event<int> && (maki::event<int> || maki::event<double>);
        REQUIRE(!es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(!es.contains<char>());
    }

    {
        constexpr auto es = !(maki::event<int> || maki::event<char>) && (maki::event<int> || maki::event<double>);
        REQUIRE(!es.contains<int>());
        REQUIRE(es.contains<double>());
        REQUIRE(!es.contains<char>());
    }
}

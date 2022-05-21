//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/tlu/left_fold.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
    template<class TList, class U>
    struct push_back_twice_helper;

    template<template<class...> class TList, class U, class... Ts>
    struct push_back_twice_helper<TList<Ts...>, U>
    {
        using type = TList<Ts..., U, U>;
    };

    template<class TList, class U>
    using push_back_twice = typename push_back_twice_helper<TList, U>::type;
}

TEST_CASE("detail::tlu::left_fold")
{
    using type_list_t = std::tuple<char, short, int, long>;

    using result_t = awesm::detail::tlu::left_fold
    <
        type_list_t,
        push_back_twice,
        std::tuple<double>
    >;

    using expected_result_t = std::tuple
    <
        double,
        char, char,
        short, short,
        int, int,
        long, long
    >;

    REQUIRE(std::is_same_v<result_t, expected_result_t>);
}

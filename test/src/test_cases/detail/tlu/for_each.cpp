//Copyright Florian Goujeon 2021.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#include <fgfsm/detail/tlu/for_each.hpp>
#include <catch2/catch.hpp>
#include <tuple>

namespace
{
#define TYPE(NAME) \
    struct NAME \
    { \
        static constexpr const char* name = #NAME; \
    };

    TYPE(type0);
    TYPE(type1);
    TYPE(type2);

#undef TYPE
}

TEST_CASE("detail::tlu::for_each")
{
    auto str = std::string{};
    fgfsm::detail::tlu::for_each<std::tuple<type0, type1, type2>>
    (
        [&str](auto* const ptr)
        {
            using type = std::remove_pointer_t<decltype(ptr)>;
            str += std::string{type::name} + ";";
        }
    );

    REQUIRE(str == "type0;type1;type2;");
}

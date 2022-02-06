//Copyright Florian Goujeon 2021 - 2022.
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

    struct for_each_helper
    {
        template<class T>
        void operator()()
        {
            str += std::string{T::name} + ";";
        }

        std::string& str;
    };
}

TEST_CASE("detail::tlu::for_each")
{
    using type_list = std::tuple<type0, type1, type2>;
    auto str = std::string{};
    auto helper = for_each_helper{str};
    fgfsm::detail::tlu::for_each<type_list>(helper);
    REQUIRE(str == "type0;type1;type2;");
}

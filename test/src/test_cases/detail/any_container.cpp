//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#include <awesm/detail/any_container.hpp>
#include "../common.hpp"
#include <array>

namespace
{
    using big_array_t = std::array<int, 10>;

    auto new_operator_call_count = 0;
    auto destructor_call_count = 0;

    struct small_struct
    {
        int i = 0;

        static void* operator new(size_t size)
        {
            ++new_operator_call_count;
            return ::operator new(size);
        }

        template<class Storage>
        static void* operator new(size_t size, Storage& storage)
        {
            return ::operator new(size, storage);
        }

        static void operator delete(void* ptr)
        {
            ::operator delete(ptr);
        }

        ~small_struct()
        {
            ++destructor_call_count;
        }
    };

    struct big_struct
    {
        big_array_t numbers;

        static void* operator new(size_t size)
        {
            ++new_operator_call_count;
            return ::operator new(size);
        }

        static void operator delete(void* ptr)
        {
            ::operator delete(ptr);
        }

        ~big_struct()
        {
            ++destructor_call_count;
        }
    };

    const auto big_array = big_array_t{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9}};
    const auto number = static_cast<char>(42);

    template<class T, auto Size>
    const T& get_as(const awesm::detail::any_container<Size>& ac)
    {
        return *reinterpret_cast<const T*>(ac.get());
    }
}

TEST_CASE("detail::any_container")
{
    destructor_call_count = 0;
    new_operator_call_count = 0;

    {
        auto psmall = new small_struct{number};
        REQUIRE(new_operator_call_count == 1);

        auto pbig = new big_struct{big_array};
        REQUIRE(new_operator_call_count == 2);

        auto small_any = awesm::detail::any_container<sizeof(small_struct)>{*psmall};
        REQUIRE(new_operator_call_count == 2);

        auto big_any = awesm::detail::any_container<sizeof(big_struct) / 2>{*pbig};
        REQUIRE(new_operator_call_count == 3);

        delete psmall;
        REQUIRE(destructor_call_count == 1);
        REQUIRE(get_as<small_struct>(small_any).i == number);

        delete pbig;
        REQUIRE(destructor_call_count == 2);
        REQUIRE(get_as<big_struct>(big_any).numbers == big_array);
    }

    REQUIRE(destructor_call_count == 4);
}

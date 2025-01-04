//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki/detail/function_queue.hpp>
#include "../common.hpp"
#include <array>

namespace
{
    using big_array_t = std::array<int, 10>;

    auto plain_new_call_count = 0;
    auto destructor_call_count = 0;

    struct small_struct
    {
        int i = 0;

        static void* operator new(size_t size)
        {
            ++plain_new_call_count;
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
            ++plain_new_call_count;
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
    const auto number = 42;

    struct copy_small_struct_content
    {
        static void call(const small_struct& in, int& out)
        {
            out = in.i;
        }
    };

    struct copy_big_struct_content
    {
        static void call(const big_struct& in, big_array_t& pout)
        {
            pout = in.numbers;
        }
    };
}

TEST_CASE("detail::function_queue")
{
    destructor_call_count = 0;
    plain_new_call_count = 0;

    {
        auto psmall = new small_struct{number};
        REQUIRE(plain_new_call_count == 1);

        auto pbig = new big_struct{big_array};
        REQUIRE(plain_new_call_count == 2);

        auto small_function_queue = maki::detail::function_queue
        <
            int&,
            sizeof(small_struct)
        >{};
        small_function_queue.push<copy_small_struct_content>(*psmall);
        REQUIRE(plain_new_call_count == 2); //no call to plain new

        auto big_function_queue = maki::detail::function_queue
        <
            big_array_t&,
            sizeof(big_struct) / 2
        >{};
        big_function_queue.push<copy_big_struct_content>(*pbig);
        REQUIRE(plain_new_call_count == 3);

        delete psmall;
        REQUIRE(destructor_call_count == 1);

        {
            auto i = 0;
            small_function_queue.invoke_and_pop_all(i);
            REQUIRE(i == number);
            REQUIRE(destructor_call_count == 2);
        }

        delete pbig;
        REQUIRE(destructor_call_count == 3);

        {
            auto arr = big_array_t{};
            big_function_queue.invoke_and_pop_all(arr);
            REQUIRE(arr == big_array);
            REQUIRE(destructor_call_count == 4);
        }
    }

    REQUIRE(destructor_call_count == 4);
}

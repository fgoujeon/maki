//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_INT_SEQUENCE_HPP
#define MAKI_DETAIL_INT_SEQUENCE_HPP

#include <utility>
#include <cstdlib>

namespace maki::detail
{

namespace int_sequence_detail
{
    template<class Operation, int Index>
    struct fold_operation_wrapper
    {
        const Operation& op;
    };

    template<class Lhs, class Operation, int Index>
    constexpr auto operator+(const Lhs& lhs, const fold_operation_wrapper<Operation, Index>& rhs)
    {
        return rhs.op(lhs, std::integral_constant<int, Index>{});
    }

    template<class IndexSequence>
    struct left_fold_helper;

    template<int... Indexes>
    struct left_fold_helper<std::integer_sequence<int, Indexes...>>
    {
        template<class Operation, class Initial>
        static constexpr auto call
        (
            const Operation& op,
            const Initial& initial
        )
        {
            return
            (
                initial +
                ... +
                int_sequence_detail::fold_operation_wrapper<Operation, Indexes>{op}
            );
        }
    };
}

/*
For each integer `N` in `[0, Size[`, invoke:

    ```
    op(r, std::integral_constant<int, N>{});
    ```

where `r` is:
- for n = 0, `initial`;
- for 0 < n < size, the result of `op(r, elems[n-1])`.
*/
template<std::size_t Size, class Operation, class Initial>
constexpr auto int_sequence_left_fold
(
    const Operation& op,
    const Initial& initial
)
{
    return int_sequence_detail::left_fold_helper<std::make_integer_sequence<int, Size>>::call
    (
        op,
        initial
    );
}

} //namespace

#endif

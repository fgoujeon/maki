//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_REMOVE_ALL_HPP
#define MAKI_DETAIL_TLU_REMOVE_ALL_HPP

#include "left_fold.hpp"
#include "push_back_if.hpp"
#include "contains.hpp"

namespace maki::detail::tlu
{

namespace remove_all_detail
{
    template<class RhsList>
    struct fold_operation_holder
    {
        template<class OutList, class Lhs>
        using operation = push_back_if_t
        <
            OutList,
            Lhs,
            !contains_v<RhsList, Lhs>
        >;
    };
}

template<class LhsList, class RhsList>
struct remove_all;

template
<
    template<class...> class LhsList,
    class... Lhss,
    class RhsList
>
struct remove_all<LhsList<Lhss...>, RhsList>
{
    using type = left_fold_t
    <
        LhsList<Lhss...>,
        remove_all_detail::fold_operation_holder<RhsList>::template operation,
        LhsList<>
    >;
};

template<class LhsList, class RhsList>
using remove_all_t = typename remove_all<LhsList, RhsList>::type;

} //namespace

#endif

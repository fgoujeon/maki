//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_LEFT_FOLD_HPP
#define AWESM_DETAIL_TLU_LEFT_FOLD_HPP

namespace awesm::detail::tlu
{

/*
left_fold applies a left fold on the given type list.

Operation must be a metafunction whose parameters are:
- a type list;
- a type.
*/

namespace left_fold_detail
{
    template
    <
        template<class, class> class Operation,
        class InitialTypeList,
        class... Ts
    >
    struct fold_on_pack;

    template
    <
        template<class, class> class Operation,
        class InitialTypeList,
        class T,
        class... Ts
    >
    struct fold_on_pack<Operation, InitialTypeList, T, Ts...>
    {
        using type = typename fold_on_pack
        <
            Operation,
            Operation<InitialTypeList, T>,
            Ts...
        >::type;
    };

    template
    <
        template<class, class> class Operation,
        class InitialTypeList
    >
    struct fold_on_pack<Operation, InitialTypeList>
    {
        using type = InitialTypeList;
    };

    template
    <
        class TList,
        template<class, class> class Operation,
        class InitialTypeList
    >
    struct fold_on_list;

    template
    <
        template<class...> class TList,
        template<class, class> class Operation,
        class InitialTypeList,
        class... Ts
    >
    struct fold_on_list<TList<Ts...>, Operation, InitialTypeList>
    {
        using type = typename fold_on_pack
        <
            Operation,
            InitialTypeList,
            Ts...
        >::type;
    };
}

template
<
    class TList,
    template<class, class> class Operation,
    class InitialTypeList
>
using left_fold = typename left_fold_detail::fold_on_list
<
    TList,
    Operation,
    InitialTypeList
>::type;

} //namespace

#endif

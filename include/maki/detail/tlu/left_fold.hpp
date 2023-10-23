//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_LEFT_FOLD_HPP
#define MAKI_DETAIL_TLU_LEFT_FOLD_HPP

namespace maki::detail::tlu
{

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
}

/*
left_fold applies a left fold on the given type list.

Operation must be a metafunction whose parameters are:
- a type list;
- a type.
*/
template
<
    class TList,
    template<class, class> class Operation,
    class InitialTypeList
>
struct left_fold;

template
<
    template<class...> class TList,
    template<class, class> class Operation,
    class InitialTypeList,
    class... Ts
>
struct left_fold<TList<Ts...>, Operation, InitialTypeList>
{
    using type = typename left_fold_detail::fold_on_pack
    <
        Operation,
        InitialTypeList,
        Ts...
    >::type;
};

template
<
    class TList,
    template<class, class> class Operation,
    class InitialTypeList
>
using left_fold_t = typename left_fold<TList, Operation, InitialTypeList>::type;

} //namespace

#endif

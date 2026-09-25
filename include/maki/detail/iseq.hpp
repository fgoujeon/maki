// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_ISEQ_HPP
#define MAKI_DETAIL_ISEQ_HPP

#include "pp/enum.hpp" // NOLINT misc-include-cleaner
#include "type_list.hpp"
#include <type_traits>

namespace maki::detail
{

// A sequence of `int`s encoded into a type
template<int... Is>
struct iseq
{
};


/*
linear_iseq_t
*/

template<int I, int N, int... Is>
struct linear_iseq_helper
{
    using type = typename linear_iseq_helper<I + 1, N, Is..., I>::type;
};

template<int N, int... Is>
struct linear_iseq_helper<N, N, Is...>
{
    using type = iseq<Is...>;
};

template<int N>
using linear_iseq_t = typename linear_iseq_helper<0, N>::type;


/*
iseq_apply_t
*/

template<class Seq, template<int...> class F>
struct iseq_apply;

template<int... Is, template<int...> class F>
struct iseq_apply<iseq<Is...>, F>
{
    using type = F<Is...>;
};

template<class Seq, template<int...> class F>
using iseq_apply_t = typename iseq_apply<Seq, F>::type;


/*
iseq_size_v
*/

template<class Seq>
struct iseq_size;

template<int... Is>
struct iseq_size<iseq<Is...>>
{
    static constexpr int value = static_cast<int>(sizeof...(Is));
};

template<class Seq>
constexpr int iseq_size_v = iseq_size<Seq>::value;


/*
iseq_empty_v
*/

template<class Seq>
constexpr int iseq_empty_v = (iseq_size_v<Seq> == 0);


/*
iseq_contains_if
*/

template<class Seq, template<int> class Predicate>
struct iseq_contains_if;

template<int... Is, template<int> class Predicate>
struct iseq_contains_if<iseq<Is...>, Predicate>
{
    static constexpr bool value = (Predicate<Is>::value || ...);
};

template<class Seq, template<int> class Predicate>
constexpr bool iseq_contains_if_v = iseq_contains_if<Seq, Predicate>::value;


/*
iseq_push_back_t
*/

template<class Seq, int I>
struct iseq_push_back;

template<int... Is, int I>
struct iseq_push_back<iseq<Is...>, I>
{
    using type = iseq<Is..., I>;
};

template<class Seq, int I>
using iseq_push_back_t = typename iseq_push_back<Seq, I>::type;


/*
iseq_push_back_if_t
*/

template<class Seq, int I, bool Condition>
struct iseq_push_back_if;

template<int... Is, int I>
struct iseq_push_back_if<iseq<Is...>, I, false>
{
    using type = iseq<Is...>;
};

template<int... Is, int I>
struct iseq_push_back_if<iseq<Is...>, I, true>
{
    using type = iseq<Is..., I>;
};

template<class Seq, int I, bool Condition>
using iseq_push_back_if_t = typename iseq_push_back_if<Seq, I, Condition>::type;


/*
iseq_push_front_if_t
*/

template<class Seq, int I, bool Condition>
struct iseq_push_front_if;

template<int... Is, int I>
struct iseq_push_front_if<iseq<Is...>, I, false>
{
    using type = iseq<Is...>;
};

template<int... Is, int I>
struct iseq_push_front_if<iseq<Is...>, I, true>
{
    using type = iseq<I, Is...>;
};

template<class Seq, int I, bool Condition>
using iseq_push_front_if_t =
    typename iseq_push_front_if<Seq, I, Condition>::type;


/*
iseq_left_fold_fn
*/

namespace iseq_left_fold_fn_detail
{
    template<class Operation, int I>
    struct operation_holder
    {
    };

    template<class Lhs, class Operation, int I>
    constexpr decltype(auto) operator+(
        const Lhs& lhs,
        const operation_holder<Operation, I>& /*rhs*/)
    {
        return Operation::template call<I>(lhs);
    }

    template<class Seq, class Operation>
    struct helper;

    template<int... Is, class Operation>
    struct helper<iseq<Is...>, Operation>
    {
        template<class Initial>
        static constexpr decltype(auto) call(const Initial& initial)
        {
            return (initial + ... + operation_holder<Operation, Is>{});
        }
    };
} // namespace iseq_left_fold_fn_detail

template<class Seq, class Operation, class Initial>
constexpr decltype(auto) iseq_left_fold_fn(const Initial& initial)
{
    return iseq_left_fold_fn_detail::helper<Seq, Operation>::call(initial);
}


/*
iseq_left_fold_t
*/

namespace iseq_left_fold_detail
{
    template<template<class, int> class Operation, class Initial, int... Is>
    struct fold_on_pack;

    template<
        template<class, int> class Operation,
        class Initial,
        int I,
        int... Is>
    struct fold_on_pack<Operation, Initial, I, Is...>
    {
        using type =
            typename fold_on_pack<Operation, Operation<Initial, I>, Is...>::
                type;
    };

    template<template<class, int> class Operation, class Initial>
    struct fold_on_pack<Operation, Initial>
    {
        using type = Initial;
    };
} // namespace iseq_left_fold_detail

template<template<class, int> class Operation, class Initial, class Seq>
struct iseq_left_fold;

template<template<class, int> class Operation, class Initial, int... Is>
struct iseq_left_fold<Operation, Initial, iseq<Is...>>
{
    using type = typename iseq_left_fold_detail::
        fold_on_pack<Operation, Initial, Is...>::type;
};

template<template<class, int> class Operation, class Initial, class Seq>
using iseq_left_fold_t = typename iseq_left_fold<Operation, Initial, Seq>::type;


/*
iseq_list_apply
*/

template<template<class...> class F, class SeqList>
struct iseq_list_apply;

template<
    template<class...> class F,
    template<class...> class SeqList,
    class... Seqs>
struct iseq_list_apply<F, SeqList<Seqs...>>
{
    using type = F<Seqs...>;
};

template<template<class...> class F, class SeqList>
using iseq_list_apply_t = typename iseq_list_apply<F, SeqList>::type;


/*
iseqs_flatten

Implementation is manually unrolled for lists of up to 10 `iseq`s to reduce the
number of template instantiations.
*/

template<class... Seqs>
struct iseqs_flatten;

#define MAKI_DETAIL_INT_TPL_PARAM(index) int... I##index##s
#define MAKI_DETAIL_INT_TPL_ARG(index) I##index##s...
#define MAKI_DETAIL_ISEQ_TPL_ARG(index) iseq<I##index##s...>

#define MAKI_DETAIL_ISEQS_FLATTEN_SPE(size) \
    template<MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_INT_TPL_PARAM)> \
    struct iseqs_flatten<MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_ISEQ_TPL_ARG)> \
    { \
        using type = \
            iseq<MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_INT_TPL_ARG)>; \
    };

MAKI_DETAIL_ISEQS_FLATTEN_SPE(1)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(2)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(3)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(4)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(5)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(6)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(7)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(8)
MAKI_DETAIL_ISEQS_FLATTEN_SPE(9)

#undef MAKI_DETAIL_ISEQS_FLATTEN_SPE

#define MAKI_DETAIL_ISEQS_FLATTEN_SPE(size) \
    template< \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_INT_TPL_PARAM), \
        class... Seqs> \
    struct iseqs_flatten< \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_ISEQ_TPL_ARG), \
        Seqs...> \
    { \
        using type = typename iseqs_flatten< \
            iseq<MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_INT_TPL_ARG)>, \
            Seqs...>::type; \
    };

MAKI_DETAIL_ISEQS_FLATTEN_SPE(10)

#undef MAKI_DETAIL_ISEQS_FLATTEN_SPE

#undef MAKI_DETAIL_INT_TPL_PARAM
#undef MAKI_DETAIL_INT_TPL_ARG
#undef MAKI_DETAIL_ISEQ_TPL_ARG

template<class... Seqs>
using iseqs_flatten_t = typename iseqs_flatten<Seqs...>::type;


/*
iseq_filter
*/

template<class Seq, template<int> class Predicate>
struct iseq_filter;

template<int... Is, template<int> class Predicate>
struct iseq_filter<iseq<Is...>, Predicate>
{
    /*
    Make a type list containing one `iseq` per integer.
    Each `iseq` contains either:
    - the current integer if `Predicate<I>::value` is true;
    - nothing otherwise.
    */
    using iseqs = type_list_t<
        std::conditional_t<Predicate<Is>::value, iseq<Is>, iseq<>>...>;

    /*
    Flatten the type list into a single `iseq`.
    */
    using type = iseq_list_apply_t<iseqs_flatten_t, iseqs>;
};

template<class Seq, template<int> class Predicate>
using iseq_filter_t = typename iseq_filter<Seq, Predicate>::type;


/*
iseq_for_each
*/

template<class Seq, class F>
struct iseq_for_each_helper
{
    template<int... Is>
    struct inner
    {
        template<class... Args>
        static void call(Args&... args)
        {
            return (F::template call<Is>(args...), ...);
        }
    };

    template<class... Args>
    static void call(Args&... args)
    {
        return iseq_apply_t<Seq, inner>::call(args...);
    }
};

template<class Seq, class F, class... Args>
void iseq_for_each(Args&... args)
{
    return iseq_for_each_helper<Seq, F>::call(args...);
}


/*
iseq_for_each_or
*/

template<class Seq, class F>
struct iseq_for_each_or_helper;

template<int... Is, class F>
struct iseq_for_each_or_helper<iseq<Is...>, F>
{
    template<class... Args>
    static constexpr bool call(Args&... args)
    {
        return (F::template call<Is>(args...) || ...);
    }
};

template<class Seq, class F, class... Args>
constexpr bool iseq_for_each_or(Args&... args)
{
    return iseq_for_each_or_helper<Seq, F>::call(args...);
}

} // namespace maki::detail

#endif

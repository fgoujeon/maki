// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_ISEQ_HPP
#define MAKI_DETAIL_ISEQ_HPP

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
    template<
        template<class, int> class Operation,
        class InitialTypeList,
        int... Is>
    struct fold_on_pack;

    template<
        template<class, int> class Operation,
        class InitialTypeList,
        int I,
        int... Is>
    struct fold_on_pack<Operation, InitialTypeList, I, Is...>
    {
        using type = typename fold_on_pack<
            Operation,
            Operation<InitialTypeList, I>,
            Is...>::type;
    };

    template<template<class, int> class Operation, class InitialTypeList>
    struct fold_on_pack<Operation, InitialTypeList>
    {
        using type = InitialTypeList;
    };
} // namespace iseq_left_fold_detail

template<class Seq, template<class, int> class Operation, class InitialTypeList>
struct iseq_left_fold;

template<template<class, int> class Operation, class InitialTypeList, int... Is>
struct iseq_left_fold<iseq<Is...>, Operation, InitialTypeList>
{
    using type = typename iseq_left_fold_detail::
        fold_on_pack<Operation, InitialTypeList, Is...>::type;
};

template<class Seq, template<class, int> class Operation, class InitialTypeList>
using iseq_left_fold_t =
    typename iseq_left_fold<Seq, Operation, InitialTypeList>::type;


/*
iseq_list_apply
*/

template<class SeqList, template<class...> class F>
struct iseq_list_apply;

template<
    template<class...> class SeqList,
    class... Seqs,
    template<class...> class F>
struct iseq_list_apply<SeqList<Seqs...>, F>
{
    using type = F<Seqs...>;
};

template<class SeqList, template<class...> class F>
using iseq_list_apply_t = typename iseq_list_apply<SeqList, F>::type;


/*
iseqs_flatten

Implementation is manually unrolled for lists of up to 10 `iseq`s to divide the
number of template instantiations by 10 for long lists.
*/

template<class... Seqs>
struct iseqs_flatten;

template<int... I0s>
struct iseqs_flatten<iseq<I0s...>>
{
    using type = iseq<I0s...>;
};

template<int... I0s, int... I1s>
struct iseqs_flatten<iseq<I0s...>, iseq<I1s...>>
{
    using type = iseq<I0s..., I1s...>;
};

template<int... I0s, int... I1s, int... I2s>
struct iseqs_flatten<iseq<I0s...>, iseq<I1s...>, iseq<I2s...>>
{
    using type = iseq<I0s..., I1s..., I2s...>;
};

template<int... I0s, int... I1s, int... I2s, int... I3s>
struct iseqs_flatten<iseq<I0s...>, iseq<I1s...>, iseq<I2s...>, iseq<I3s...>>
{
    using type = iseq<I0s..., I1s..., I2s..., I3s...>;
};

template<int... I0s, int... I1s, int... I2s, int... I3s, int... I4s>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>>
{
    using type = iseq<I0s..., I1s..., I2s..., I3s..., I4s...>;
};

template<int... I0s, int... I1s, int... I2s, int... I3s, int... I4s, int... I5s>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>,
    iseq<I5s...>>
{
    using type = iseq<I0s..., I1s..., I2s..., I3s..., I4s..., I5s...>;
};

template<
    int... I0s,
    int... I1s,
    int... I2s,
    int... I3s,
    int... I4s,
    int... I5s,
    int... I6s>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>,
    iseq<I5s...>,
    iseq<I6s...>>
{
    using type = iseq<I0s..., I1s..., I2s..., I3s..., I4s..., I5s..., I6s...>;
};

template<
    int... I0s,
    int... I1s,
    int... I2s,
    int... I3s,
    int... I4s,
    int... I5s,
    int... I6s,
    int... I7s>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>,
    iseq<I5s...>,
    iseq<I6s...>,
    iseq<I7s...>>
{
    using type =
        iseq<I0s..., I1s..., I2s..., I3s..., I4s..., I5s..., I6s..., I7s...>;
};

template<
    int... I0s,
    int... I1s,
    int... I2s,
    int... I3s,
    int... I4s,
    int... I5s,
    int... I6s,
    int... I7s,
    int... I8s>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>,
    iseq<I5s...>,
    iseq<I6s...>,
    iseq<I7s...>,
    iseq<I8s...>>
{
    using type = iseq<
        I0s...,
        I1s...,
        I2s...,
        I3s...,
        I4s...,
        I5s...,
        I6s...,
        I7s...,
        I8s...>;
};

template<
    int... I0s,
    int... I1s,
    int... I2s,
    int... I3s,
    int... I4s,
    int... I5s,
    int... I6s,
    int... I7s,
    int... I8s,
    int... I9s,
    class... Seqs>
struct iseqs_flatten<
    iseq<I0s...>,
    iseq<I1s...>,
    iseq<I2s...>,
    iseq<I3s...>,
    iseq<I4s...>,
    iseq<I5s...>,
    iseq<I6s...>,
    iseq<I7s...>,
    iseq<I8s...>,
    iseq<I9s...>,
    Seqs...>
{
    using type = typename iseqs_flatten<
        iseq<
            I0s...,
            I1s...,
            I2s...,
            I3s...,
            I4s...,
            I5s...,
            I6s...,
            I7s...,
            I8s...,
            I9s...>,
        Seqs...>::type;
};

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
    using type = iseq_list_apply_t<iseqs, iseqs_flatten_t>;
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

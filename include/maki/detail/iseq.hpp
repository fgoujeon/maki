//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_ISEQ_HPP
#define MAKI_DETAIL_ISEQ_HPP

namespace maki::detail
{

// A sequence of `int`s encoded into a type
template<int... Is>
struct iseq{};


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
using iseq_push_front_if_t = typename iseq_push_front_if<Seq, I, Condition>::type;


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
    constexpr decltype(auto) operator+(const Lhs& lhs, const operation_holder<Operation, I>& /*rhs*/)
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
}

template
<
    class Seq,
    class Operation,
    class Initial
>
constexpr decltype(auto) iseq_left_fold_fn(const Initial& initial)
{
    return iseq_left_fold_fn_detail::helper<Seq, Operation>::call(initial);
}


/*
iseq_left_fold_t
*/

namespace iseq_left_fold_detail
{
    template
    <
        template<class, int> class Operation,
        class InitialTypeList,
        int... Is
    >
    struct fold_on_pack;

    template
    <
        template<class, int> class Operation,
        class InitialTypeList,
        int I,
        int... Is
    >
    struct fold_on_pack<Operation, InitialTypeList, I, Is...>
    {
        using type = typename fold_on_pack
        <
            Operation,
            Operation<InitialTypeList, I>,
            Is...
        >::type;
    };

    template
    <
        template<class, int> class Operation,
        class InitialTypeList
    >
    struct fold_on_pack<Operation, InitialTypeList>
    {
        using type = InitialTypeList;
    };
}

template
<
    class Seq,
    template<class, int> class Operation,
    class InitialTypeList
>
struct iseq_left_fold;

template
<
    template<class, int> class Operation,
    class InitialTypeList,
    int... Is
>
struct iseq_left_fold<iseq<Is...>, Operation, InitialTypeList>
{
    using type = typename iseq_left_fold_detail::fold_on_pack
    <
        Operation,
        InitialTypeList,
        Is...
    >::type;
};

template
<
    class Seq,
    template<class, int> class Operation,
    class InitialTypeList
>
using iseq_left_fold_t = typename iseq_left_fold<Seq, Operation, InitialTypeList>::type;


/*
iseq_filter
*/

template
<
    class Seq,
    template<int> class Predicate
>
struct iseq_filter;

template
<
    int I,
    int... Is,
    template<int> class Predicate
>
struct iseq_filter<iseq<I, Is...>, Predicate>
{
    using type = iseq_push_front_if_t
    <
        typename iseq_filter<iseq<Is...>, Predicate>::type,
        I,
        Predicate<I>::value
    >;
};

template
<
    template<int> class Predicate
>
struct iseq_filter<iseq<>, Predicate>
{
    using type = iseq<>;
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
        return iseq_apply_t
        <
            Seq,
            inner
        >::call(args...);
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

template<class R, class Seq, class F>
struct iseq_for_each_or_helper;

template<class R, int... Is, class F>
struct iseq_for_each_or_helper<R, iseq<Is...>, F>
{
    template<class... Args>
    static constexpr R call(Args&... args)
    {
        return (F::template call<Is>(args...) || ...);
    }
};

template<class R, class Seq, class F, class... Args>
constexpr bool iseq_for_each_or(Args&... args)
{
    return iseq_for_each_or_helper<R, Seq, F>::call(args...);
}

#ifdef __cpp_impl_coroutine
/*
async_iseq_for_each_or
*/

template<class R, class Seq, class F>
struct async_iseq_for_each_or_helper;

template<class R, class F>
struct async_iseq_for_each_or_helper<R, iseq<>, F>
{
    template<class... Args>
    static R call(Args&... /*args*/)
    {
        co_return false;
    }
};

template<class R, int I, int... Is, class F>
struct async_iseq_for_each_or_helper<R, iseq<I, Is...>, F>
{
    template<class... Args>
    static R call(Args&... args)
    {
        if(co_await F::template call<I>(args...))
        {
            co_return true;
        }

        co_return co_await async_iseq_for_each_or_helper<R, iseq<Is...>, F>::call(args...);
    }
};

template<class R, class Seq, class F, class... Args>
R async_iseq_for_each_or(Args&... args)
{
    co_return co_await async_iseq_for_each_or_helper<R, Seq, F>::call(args...);
}
#endif

} //namespace

#endif

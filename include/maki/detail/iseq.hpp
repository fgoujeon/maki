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
iseq_left_fold
*/

namespace iseq_left_fold_detail
{
    template<class Operation, int I>
    struct operation_holder
    {
    };

    template<class Lhs, class Operation, int I>
    constexpr auto operator+(const Lhs& lhs, const operation_holder<Operation, I>& /*rhs*/)
    {
        return Operation::template call<I>(lhs);
    }

    template<class Seq, class Operation>
    struct helper;

    template<int... Is, class Operation>
    struct helper<iseq<Is...>, Operation>
    {
        template<class Initial>
        static constexpr auto call(const Initial& initial)
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
constexpr auto iseq_left_fold(const Initial& initial)
{
    return iseq_left_fold_detail::helper<Seq, Operation>::call(initial);
}


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
iseq_for_each_or
*/

template<class Seq, class F>
struct iseq_for_each_or_helper
{
    template<int... Is>
    struct inner
    {
        template<class... Args>
        static bool call(Args&... args)
        {
            return (F::template call<Is>(args...) || ...);
        }
    };

    template<class... Args>
    static bool call(Args&... args)
    {
        return iseq_apply_t
        <
            Seq,
            inner
        >::call(args...);
    }
};

template<class Seq, class F, class... Args>
bool iseq_for_each_or(Args&... args)
{
    return iseq_for_each_or_helper<Seq, F>::call(args...);
}

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_INDEX_SEQUENCE_HPP
#define MAKI_DETAIL_INDEX_SEQUENCE_HPP

namespace maki::detail
{

template<int... Is>
struct index_sequence{};


/*
index_sequence_apply_t
*/

template<class Seq, template<int...> class F>
struct index_sequence_apply;

template<int... Is, template<int...> class F>
struct index_sequence_apply<index_sequence<Is...>, F>
{
    using type = F<Is...>;
};

template<class Seq, template<int...> class F>
using index_sequence_apply_t = typename index_sequence_apply<Seq, F>::type;


/*
index_sequence_size_v
*/

template<class Seq>
struct index_sequence_size;

template<int... Is>
struct index_sequence_size<index_sequence<Is...>>
{
    static constexpr int value = static_cast<int>(sizeof...(Is));
};

template<class Seq>
constexpr int index_sequence_size_v = index_sequence_size<Seq>::value;


/*
index_sequence_push_back_t
*/

template<class Seq, int I>
struct index_sequence_push_back;

template<int... Is, int I>
struct index_sequence_push_back<index_sequence<Is...>, I>
{
    using type = index_sequence<Is..., I>;
};

template<class Seq, int I>
using index_sequence_push_back_t = typename index_sequence_push_back<Seq, I>::type;


/*
index_sequence_push_back_if_t
*/

template<class Seq, int I, bool Condition>
struct index_sequence_push_back_if;

template<int... Is, int I>
struct index_sequence_push_back_if<index_sequence<Is...>, I, false>
{
    using type = index_sequence<Is...>;
};

template<int... Is, int I>
struct index_sequence_push_back_if<index_sequence<Is...>, I, true>
{
    using type = index_sequence<Is..., I>;
};

template<class Seq, int I, bool Condition>
using index_sequence_push_back_if_t = typename index_sequence_push_back_if<Seq, I, Condition>::type;


/*
index_sequence_push_front_if_t
*/

template<class Seq, int I, bool Condition>
struct index_sequence_push_front_if;

template<int... Is, int I>
struct index_sequence_push_front_if<index_sequence<Is...>, I, false>
{
    using type = index_sequence<Is...>;
};

template<int... Is, int I>
struct index_sequence_push_front_if<index_sequence<Is...>, I, true>
{
    using type = index_sequence<I, Is...>;
};

template<class Seq, int I, bool Condition>
using index_sequence_push_front_if_t = typename index_sequence_push_front_if<Seq, I, Condition>::type;


/*
index_sequence_left_fold
*/

namespace index_sequence_left_fold_detail
{
    template<class Operation, int Index>
    struct operation_holder
    {
    };

    template<class Lhs, class Operation, int Index>
    constexpr auto operator+(const Lhs& lhs, const operation_holder<Operation, Index>& /*rhs*/)
    {
        return Operation::template call<Index>(lhs);
    }

    template<class Seq, class Operation>
    struct helper;

    template<int... Is, class Operation>
    struct helper<index_sequence<Is...>, Operation>
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
constexpr auto index_sequence_left_fold(const Initial& initial)
{
    return index_sequence_left_fold_detail::helper<Seq, Operation>::call(initial);
}


/*
index_sequence_filter
*/

template
<
    class Seq,
    template<int> class Predicate
>
struct index_sequence_filter;

template
<
    int I,
    int... Is,
    template<int> class Predicate
>
struct index_sequence_filter<index_sequence<I, Is...>, Predicate>
{
    using type = index_sequence_push_front_if_t
    <
        typename index_sequence_filter<index_sequence<Is...>, Predicate>::type,
        I,
        Predicate<I>::value
    >;
};

template
<
    template<int> class Predicate
>
struct index_sequence_filter<index_sequence<>, Predicate>
{
    using type = index_sequence<>;
};

template<class Seq, template<int> class Predicate>
using index_sequence_filter_t = typename index_sequence_filter<Seq, Predicate>::type;


/*
index_sequence_for_each_or
*/

template<class Seq, class F>
struct index_sequence_for_each_or_helper
{
    template<int... Indexes>
    struct inner
    {
        template<class... Args>
        static bool call(Args&... args)
        {
            return (F::template call<Indexes>(args...) || ...);
        }
    };

    template<class... Args>
    static bool call(Args&... args)
    {
        return index_sequence_apply_t
        <
            Seq,
            inner
        >::call(args...);
    }
};

template<class Seq, class F, class... Args>
bool index_sequence_for_each_or(Args&... args)
{
    return index_sequence_for_each_or_helper<Seq, F>::call(args...);
}

} //namespace

#endif

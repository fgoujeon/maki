//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TUPLE_2_HPP
#define MAKI_DETAIL_TUPLE_2_HPP

#include "constant.hpp"
#include <functional>
#include <utility>
#include <type_traits>

namespace maki::detail
{

/*
A minimal std::tuple-like container.
Using this instead of std::tuple improves build time.
*/
template<class... Ts>
struct tuple_2;

template<class T, class... Ts>
struct tuple_2<T, Ts...>
{
    T head;
    tuple_2<Ts...> tail;
};

template<>
struct tuple_2<>
{
};

inline constexpr auto empty_tuple_c = tuple_2<>{};

constexpr tuple_2<> make_tuple_2()
{
    return tuple_2<>{};
}

template<class T, class... Ts>
constexpr tuple_2<T, Ts...> make_tuple_2(const T& elem, const Ts&... elems)
{
    return tuple_2<T, Ts...>{elem, make_tuple_2(elems...)};
}


/*
get
*/

template<std::size_t Index, class... Ts>
constexpr const auto& get(const tuple_2<Ts...>& tpl)
{
    if constexpr(Index == 0)
    {
        return tpl.head;
    }
    else
    {
        return get<Index - 1>(tpl.tail);
    }
}


/*
size
*/

template<class... Ts>
constexpr std::size_t size(const tuple_2<Ts...>& /*tpl*/)
{
    return sizeof...(Ts);
}


/*
head
*/

template<const auto& Tuple>
inline constexpr auto head_c = Tuple.head;


/*
apply
*/

template<class IndexSequence>
struct apply_helper;

template<std::size_t... Indexes>
struct apply_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static constexpr auto call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        return std::forward<F>(fun)
        (
            get<Indexes>(tpl)...,
            std::forward<Args>(args)...
        );
    }
};

template<class F, class... Ts, class... Args>
constexpr auto apply(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    using index_sequence_t = std::make_index_sequence<sizeof...(Ts)>;
    return apply_helper<index_sequence_t>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
tail
*/

template<const auto& Tuple>
constexpr auto tail_c = Tuple.tail;


/*
for_each_element
*/

template<class IndexSequence>
struct for_each_element_helper;

template<std::size_t... Indexes>
struct for_each_element_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static void call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        (
            fun
            (
                get<Indexes>(tpl),
                std::forward<Args>(args)...
            ),
            ...
        );
    }
};

template<class F, class... Ts, class... Args>
void for_each_element(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    for_each_element_helper<std::make_index_sequence<sizeof...(Ts)>>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
for_each_element_or
*/

template<class IndexSequence>
struct for_each_element_or_helper;

template<std::size_t... Indexes>
struct for_each_element_or_helper<std::index_sequence<Indexes...>>
{
    template<class F, class... Ts, class... Args>
    static constexpr bool call(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
    {
        return
        (
            fun
            (
                get<Indexes>(tpl),
                std::forward<Args>(args)...
            ) ||
            ...
        );
    }
};

template<class F, class... Ts, class... Args>
constexpr bool for_each_element_or(F&& fun, const tuple_2<Ts...>& tpl, Args&&... args)
{
    return for_each_element_or_helper<std::make_index_sequence<sizeof...(Ts)>>::call
    (
        std::forward<F>(fun),
        tpl,
        std::forward<Args>(args)...
    );
}


/*
left_fold
*/

template<class Operation, class OutputValue, class... Ts>
constexpr OutputValue left_fold
(
    const Operation& op,
    const OutputValue& initial_value,
    const tuple_2<Ts...>& tpl
)
{
    if constexpr(sizeof...(Ts) == 0)
    {
        return initial_value;
    }
    else
    {
        return left_fold(op, op(initial_value, tpl.head), tpl.tail);
    }
}

template
<
    class Operation,
    class InitialValueRefConstant,
    class TupleRefConstant
>
constexpr auto left_fold_const
(
    const InitialValueRefConstant initial_value_ref_constant,
    const TupleRefConstant
)
{
    if constexpr(size(TupleRefConstant::value) == 0)
    {
        return initial_value_ref_constant;
    }
    else
    {
        return left_fold_const<Operation>
        (
            Operation::call
            (
                initial_value_ref_constant,
                constant_reference_c<head_c<TupleRefConstant::value>>
            ),
            constant_reference_c<tail_c<TupleRefConstant::value>>
        );
    }
}


/*
push_back
*/

template<class... Ts, class U>
constexpr auto push_back(const tuple_2<Ts...>& tpl, const U& elem)
{
    return apply
    (
        make_tuple_2<Ts..., U>,
        tpl,
        elem
    );
}

template<class TupleRefConstant, class ElemRefConstant>
inline constexpr auto push_back_c = push_back(TupleRefConstant::value, ElemRefConstant::value);


/*
push_back_if
*/

template<bool Condition, class... Ts, class U>
constexpr auto push_back_if(const tuple_2<Ts...>& tpl, const U& elem)
{
    if constexpr(Condition)
    {
        return push_back(tpl, elem);
    }
    else
    {
        return tpl;
    }
}



/*
filter
*/

template<class Predicate>
struct filter_operation
{
    template
    <
        class OutputTupleRefConstant,
        class InputTupleElemRefConstant
    >
    static constexpr auto call
    (
        const OutputTupleRefConstant output_tuple_ref_constant,
        const InputTupleElemRefConstant /**/
    )
    {
        if constexpr(Predicate::call(InputTupleElemRefConstant::value))
        {
            return constant_reference_c<push_back_c<OutputTupleRefConstant, InputTupleElemRefConstant>>;
        }
        else
        {
            return output_tuple_ref_constant;
        }
    }
};

template<const auto& Tuple, class Predicate>
constexpr auto filter()
{
    return left_fold_const<filter_operation<Predicate>>
    (
        constant_reference_c<empty_tuple_c>, //Initial OutputTuple
        constant_reference_c<Tuple> //InputTuple
    )();
}

} //namespace

#endif

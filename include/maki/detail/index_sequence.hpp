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

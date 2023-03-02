//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TLU_SET_HPP
#define AWESM_DETAIL_TLU_SET_HPP

namespace awesm::detail::tlu
{

template<class TList, int Index, class U>
struct set_at;

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 0, U>
{
    using type = TList<U, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 1, U>
{
    using type = TList<T0, U, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 2, U>
{
    using type = TList<T0, T1, U, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 3, U>
{
    using type = TList<T0, T1, T2, U, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 4, U>
{
    using type = TList<T0, T1, T2, T3, U, T5, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 5, U>
{
    using type = TList<T0, T1, T2, T3, T4, U, T6, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 6, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, U, T7, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 7, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, U, T8, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 8, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, U, T9, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 9, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, U, T10, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 10, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, U, T11, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 11, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, U, T12, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 12, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, U, T13>;
};

template
<
    template<class...> class TList,
    class T0,
    class T1,
    class T2,
    class T3,
    class T4,
    class T5,
    class T6,
    class T7,
    class T8,
    class T9,
    class T10,
    class T11,
    class T12,
    class T13,
    class U
>
struct set_at<TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13>, 13, U>
{
    using type = TList<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, U>;
};

template<class TList, int Index, class U>
using set_at_t = typename set_at<TList, Index, U>::type;

template<class TList, auto Index, class U>
using set_at_f_t = set_at_t<TList, static_cast<int>(Index), U>;

} //namespace

#endif

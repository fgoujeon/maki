// Copyright Florian Goujeon 2021 - 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// https://www.boost.org/LICENSE_1_0.txt)
// Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_LEFT_FOLD_HPP
#define MAKI_DETAIL_TLU_LEFT_FOLD_HPP

#include "../pp/enum.hpp" // NOLINT misc-include-cleaner
#include "../pp/for.hpp" // NOLINT misc-include-cleaner

namespace maki::detail::tlu
{

namespace left_fold_detail
{
    /*
    Implementation is manually unrolled for lists of up to 10 types to reduce
    the number of template instantiations.
    */
    template<class V, template<class, class> class F, class... Ts>
    struct fold_impl;

    template<class V, template<class, class> class F>
    struct fold_impl<V, F>
    {
        using type = V;
    };

    // NOLINTBEGIN cppcoreguidelines-macro-usage

#define MAKI_DETAIL_TYPE_TPL_PARAM(index) class T##index
#define MAKI_DETAIL_TYPE_TPL_ARG(index) T##index
#define MAKI_DETAIL_TYPE_ANGLE(index) T##index >
#define MAKI_DETAIL_F_ANGLE(index) F <

#define MAKI_DETAIL_FOLD_SPE(size) \
    template< \
        class V, \
        template<class, class> class F, \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_TPL_PARAM)> \
    struct fold_impl< \
        V, \
        F, \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_TPL_ARG)> \
    { \
        using type = MAKI_DETAIL_PP_FOR_##size(MAKI_DETAIL_F_ANGLE) V, \
              MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_ANGLE); \
    };

    MAKI_DETAIL_FOLD_SPE(1)
    MAKI_DETAIL_FOLD_SPE(2)
    MAKI_DETAIL_FOLD_SPE(3)
    MAKI_DETAIL_FOLD_SPE(4)
    MAKI_DETAIL_FOLD_SPE(5)
    MAKI_DETAIL_FOLD_SPE(6)
    MAKI_DETAIL_FOLD_SPE(7)
    MAKI_DETAIL_FOLD_SPE(8)
    MAKI_DETAIL_FOLD_SPE(9)

#undef MAKI_DETAIL_FOLD_SPE

#define MAKI_DETAIL_FOLD_SPE(size) \
    template< \
        class V, \
        template<class, class> class F, \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_TPL_PARAM), \
        class... Ts> \
    struct fold_impl< \
        V, \
        F, \
        MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_TPL_ARG), \
        Ts...> \
    { \
        using type = typename fold_impl< \
            MAKI_DETAIL_PP_FOR_##size(MAKI_DETAIL_F_ANGLE) V, \
            MAKI_DETAIL_PP_ENUM_##size(MAKI_DETAIL_TYPE_ANGLE), \
            F, \
            Ts...>::type; \
    };

    MAKI_DETAIL_FOLD_SPE(10)

#undef MAKI_DETAIL_FOLD_SPE

#undef MAKI_DETAIL_TYPE_TPL_PARAM
#undef MAKI_DETAIL_TYPE_TPL_ARG
#undef MAKI_DETAIL_TYPE_ANGLE
#undef MAKI_DETAIL_F_ANGLE

    // NOLINTEND cppcoreguidelines-macro-usage

} // namespace left_fold_detail

/*
left_fold applies a left fold on the given type list.
*/
template<class Initial, template<class, class> class Operation, class TList>
struct left_fold;

template<
    class Initial,
    template<class, class> class Operation,
    template<class...> class TList,
    class... Ts>
struct left_fold<Initial, Operation, TList<Ts...>>
{
    using type =
        typename left_fold_detail::fold_impl<Initial, Operation, Ts...>::type;
};

template<class Initial, template<class, class> class Operation, class TList>
using left_fold_t = typename left_fold<Initial, Operation, TList>::type;

} // namespace maki::detail::tlu

#endif

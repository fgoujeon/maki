//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_ALTERNATIVE_LAZY_HPP
#define FGFSM_DETAIL_ALTERNATIVE_LAZY_HPP

namespace fgfsm::detail
{

/*
alternative_lazy returns TrueTypeHolder::type<> if given B is true,
FalseTypeHolder::type<> otherwise.
TrueTypeHolder::type<> is instantiated only if B is true.
FalseTypeHolder::type<> is instantiated only if B is false.

Example:
    struct true_type
    {
        template<class = void>
        using type = expensive_computation<>;
    };

    struct false_type
    {
        template<class = void>
        using type = another_expensive_computation<>;
    };

    using result_t = alternative_lazy<true_or_false, true_type, false_type>;
*/

namespace alternative_lazy_detail
{
    template<bool B, class TrueTypeHolder, class FalseTypeHolder>
    struct helper;

    template<class TrueTypeHolder, class FalseTypeHolder>
    struct helper<true, TrueTypeHolder, FalseTypeHolder>
    {
        using type = typename TrueTypeHolder::template type<>;
    };

    template<class TrueTypeHolder, class FalseTypeHolder>
    struct helper<false, TrueTypeHolder, FalseTypeHolder>
    {
        using type = typename FalseTypeHolder::template type<>;
    };
}

template<bool B, class TrueTypeHolder, class FalseTypeHolder>
using alternative_lazy =
    typename alternative_lazy_detail::helper<B, TrueTypeHolder, FalseTypeHolder>::type
;

} //namespace

#endif

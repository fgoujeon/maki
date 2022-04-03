//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/fgfsm

#ifndef FGFSM_DETAIL_ALTERNATIVE_HPP
#define FGFSM_DETAIL_ALTERNATIVE_HPP

namespace fgfsm::detail
{

/*
alternative returns TrueType if given B is true, FalseType otherwise.
*/

namespace alternative_detail
{
    template<bool B, class TrueType, class FalseType>
    struct helper;

    template<class TrueType, class FalseType>
    struct helper<true, TrueType, FalseType>
    {
        using type = TrueType;
    };

    template<class TrueType, class FalseType>
    struct helper<false, TrueType, FalseType>
    {
        using type = FalseType;
    };
}

template<bool B, class TrueType, class FalseType>
using alternative =
    typename alternative_detail::helper<B, TrueType, FalseType>::type
;

} //namespace

#endif

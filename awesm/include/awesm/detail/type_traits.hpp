//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_TYPE_TRAITS_HPP
#define AWESM_DETAIL_TYPE_TRAITS_HPP

namespace awesm
{
    template<class Definition, class RegionList>
    class composite_state;
}

namespace awesm::detail
{

template<class T>
struct is_composite_state
{
    static constexpr auto value = false;
};

template<class Definition, class RegionList>
struct is_composite_state<composite_state<Definition, RegionList>>
{
    static constexpr auto value = true;
};

template<class T>
constexpr auto is_composite_state_v = is_composite_state<T>::value;

} //namespace

#endif

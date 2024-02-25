//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STORABLE_CONF_FILTER_HPP
#define MAKI_DETAIL_STORABLE_CONF_FILTER_HPP

#include "conf_traits.hpp"
#include "ref_wrapper.hpp"

namespace maki::detail
{

/*
`*_conf` instances need to be wrapped into a `cref_wrapper` because what
identifies a state is the address of its conf, not the value of the fields of
its conf.
*/

template<class T>
struct storable_conf_filter
{
    using type = T;
};

template<class T>
using storable_conf_filter_t = typename storable_conf_filter<T>::type;

template<class OptionSet>
struct storable_conf_filter<state_conf<OptionSet>>
{
    using type = ref_wrapper<const state_conf<OptionSet>>;
};

template<class OptionSet>
struct storable_conf_filter<submachine_conf<OptionSet>>
{
    using type = ref_wrapper<const submachine_conf<OptionSet>>;
};

template<class T>
constexpr auto to_storable_conf_filter(const T& conf)
{
    if constexpr
    (
        conf_traits::is_state_conf_v<T> ||
        conf_traits::is_submachine_conf_v<T>
    )
    {
        return make_cref_wrapper(conf);
    }
    else
    {
        return conf;
    }
}

} //namespace

#endif

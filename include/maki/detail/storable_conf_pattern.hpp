//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STORABLE_CONF_PATTERN_HPP
#define MAKI_DETAIL_STORABLE_CONF_PATTERN_HPP

#include "conf_traits.hpp"
#include "cref_wrapper.hpp"

namespace maki::detail
{

/*
`*_conf` instances need to be wrapped into a `cref_wrapper` because what
identifies a state is the address of its conf, not the value of the fields of
its conf.
*/

template<class T>
struct storable_conf_pattern
{
    using type = T;
};

template<class T>
using storable_conf_pattern_t = typename storable_conf_pattern<T>::type;

template<class OptionSet>
struct storable_conf_pattern<state_conf<OptionSet>>
{
    using type = cref_wrapper<state_conf<OptionSet>>;
};

template<class OptionSet>
struct storable_conf_pattern<submachine_conf<OptionSet>>
{
    using type = cref_wrapper<submachine_conf<OptionSet>>;
};

template<class T>
constexpr auto to_storable_conf_pattern(const T& conf)
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

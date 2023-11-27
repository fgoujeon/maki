//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_CONF_WRAPPER_HPP
#define MAKI_DETAIL_STATE_CONF_WRAPPER_HPP

namespace maki::detail
{

/*
Temporary
Wraps a conf variable into a type that looks like an old-style state type.
*/
template<const auto& Conf>
struct state_conf_wrapper
{
    static constexpr const auto& conf = Conf;
};

} //namespace

#endif

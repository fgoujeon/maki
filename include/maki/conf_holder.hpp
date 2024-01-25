//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine class template
*/

#ifndef MAKI_CONF_HOLDER_HPP
#define MAKI_CONF_HOLDER_HPP

namespace maki
{

template<const auto& Conf>
struct conf_holder
{
    static constexpr const auto& conf = Conf;
};

} //namespace

#endif

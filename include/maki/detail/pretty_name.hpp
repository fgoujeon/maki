//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::pretty_name function template
*/

#ifndef MAKI_DETAIL_PRETTY_NAME_HPP
#define MAKI_DETAIL_PRETTY_NAME_HPP

#include "machine_conf_tree.hpp"
#include "type_name.hpp"

namespace maki::detail
{

/**
@brief Gets the pretty name of `maki::state_mold`.
*/
template<class MachineConfHolder, class StateMoldPath>
decltype(auto) pretty_name()
{
    constexpr const auto& stt_mold = machine_conf_tree::node_at_path_v<MachineConfHolder, StateMoldPath>;

    if constexpr(impl_of(stt_mold).pretty_name.data() == nullptr)
    {
        return detail::decayed_constant_name<stt_mold>();
    }
    else
    {
        return impl_of(stt_mold).pretty_name;
    }
}

} //namespace

#endif

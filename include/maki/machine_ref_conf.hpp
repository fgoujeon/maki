//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine_ref_conf struct template
*/

#ifndef MAKI_MACHINE_REF_CONF_HPP
#define MAKI_MACHINE_REF_CONF_HPP

#include "detail/type_list.hpp"

namespace maki
{

#if MAKI_DETAIL_DOXYGEN
/**
@brief The configuration for `maki::machine_ref`
*/
template<IMPLEMENTATION_DETAIL>
#else
template<class EventTypeList = detail::type_list_t<>>
#endif
struct machine_ref_conf
{
    machine_ref_conf() = default;

    machine_ref_conf(const machine_ref_conf&) = delete;

    machine_ref_conf(machine_ref_conf&&) = delete;

    ~machine_ref_conf() = default;

    machine_ref_conf& operator=(const machine_ref_conf&) = delete;

    machine_ref_conf& operator=(machine_ref_conf&&) = delete;

    template<class... Events>
    [[nodiscard]] constexpr auto events() const
    {
        return machine_ref_conf<detail::type_list_t<Events...>>{};
    }

#if MAKI_DETAIL_DOXYGEN
private:
#endif
    using event_type_list = EventTypeList;
};

} //namespace

#endif

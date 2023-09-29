//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_MACHINE_REF_CONF_HPP
#define MAKI_MACHINE_REF_CONF_HPP

#include "detail/tlu.hpp"
#include "detail/type_list.hpp"
#include "detail/type.hpp"

namespace maki
{

template<class EventTypeList = detail::type_list<>>
struct machine_ref_conf
{
    EventTypeList on_event;

    template<class... Ts>
    [[nodiscard]] constexpr auto enable_on_event() const
    {
        return machine_ref_conf<detail::type_list<Ts...>>{};
    }
};

inline constexpr auto machine_ref_conf_c = machine_ref_conf<>{};

} //namespace

#endif

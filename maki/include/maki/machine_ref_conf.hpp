//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_MACHINE_REF_CONF_HPP
#define MAKI_MACHINE_REF_CONF_HPP

#include "detail/constant.hpp"
#include "detail/tlu.hpp"
#include "detail/type.hpp"
#include "detail/conf.hpp"

namespace maki
{

namespace machine_ref_opts
{
    template<class... Events>
    using events = detail::conf_element<detail::option_id::events, detail::type_list<Events...>>;
}

template<class... Options>
struct machine_ref_conf_tpl
{
    template<class... Events>
    using events = machine_ref_conf_tpl<Options..., machine_ref_opts::events<Events...>>;
};

using machine_ref_conf = machine_ref_conf_tpl<>;

} //namespace

#endif

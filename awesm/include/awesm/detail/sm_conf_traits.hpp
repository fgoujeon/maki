//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_CONF_TRAITS_HPP
#define AWESM_DETAIL_SM_CONF_TRAITS_HPP

#include "tlu.hpp"
#include "overload_priority.hpp"
#include "type_tag.hpp"
#include "../sm_conf.hpp"
#include <type_traits>

namespace awesm::detail::sm_conf_traits
{

namespace context_detail
{
    template<class SmConf, class DefaultContext>
    inline auto get_context(overload_priority::low /*unused*/)
    {
        return type_tag<DefaultContext>{};
    }

    template
    <
        class SmConf,
        class DefaultContext,
        std::enable_if_t<!std::is_void_v<tlu::get_f_t<SmConf, sm_option::context>>, bool> = true
    >
    inline auto get_context(overload_priority::high /*unused*/)
    {
        return type_tag<tlu::get_f_t<SmConf, sm_option::context>>{};
    }
}

template<class SmConf, class DefaultContext>
using context_t = typename decltype(context_detail::get_context<SmConf, DefaultContext>(overload_priority::probe))::type;

} //namespace

#endif

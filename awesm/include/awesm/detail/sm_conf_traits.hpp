//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_SM_CONF_TRAITS_HPP
#define AWESM_DETAIL_SM_CONF_TRAITS_HPP

#include "clu.hpp"
#include "overload_priority.hpp"
#include "type_tag.hpp"
#include "../transition_table.hpp"
#include <type_traits>

namespace awesm::detail::sm_conf_traits
{

namespace transition_table_fn_list_detail
{
    template<class R>
    using nullary_fn_ptr_t = R(*)();

    /*
    T is either:
    - a pointer to a function returning an instance of transition_table_t;
    - a pointer to an instance of transition_table_list_t.
    */
    template<class T, auto V>
    struct helper;

    template<class... Transitions, auto TransitionTableFn>
    struct helper<nullary_fn_ptr_t<transition_table_t<Transitions...>>, TransitionTableFn>
    {
        using type = transition_table_list_t<TransitionTableFn>;
    };

    template<auto... TransitionTableFns, auto V>
    struct helper<const transition_table_list_t<TransitionTableFns...>*, V>
    {
        using type = transition_table_list_t<TransitionTableFns...>;
    };
}

namespace context_detail
{
    template<class SmConf, class DefaultContext>
    inline auto get_context(overload_priority::low /*unused*/)
    {
        return type_tag<DefaultContext>{};
    }

    template<class SmConf, class DefaultContext>
    inline type_tag<typename SmConf::option_mix_type::context_type> get_context(overload_priority::high /*unused*/)
    {
        return type_tag<typename SmConf::option_mix_type::context_type>{};
    }
}

template<class SmConf, class DefaultContext>
using context_t = typename decltype(context_detail::get_context<SmConf, DefaultContext>(overload_priority::probe))::type;

template<class SmConf>
using transition_table_fn_list_t = typename transition_table_fn_list_detail::helper
<
    std::decay_t<decltype(SmConf::transition_table_fn)>,
    SmConf::transition_table_fn
>::type;

template<class SmConf>
inline constexpr auto region_count_v = clu::size_v<transition_table_fn_list_t<SmConf>>;

} //namespace

#endif

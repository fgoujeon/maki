//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_TRANSITION_TABLE_HPP
#define AWESM_TRANSITION_TABLE_HPP

#include <type_traits>

namespace awesm
{

template<class... Rows>
struct transition_table_t{};

template<auto... Fns>
struct transition_table_list_t{};

template<class... Rows>
inline constexpr auto transition_table = transition_table_t<Rows...>{};

template<auto... Fns>
inline constexpr auto transition_table_list = transition_table_list_t<Fns...>{};

namespace detail
{
    template<class T, const auto& V>
    struct to_transition_table_fn_list_helper;

    template<class... Rows, const auto& TransitionTableFn>
    struct to_transition_table_fn_list_helper<transition_table_t<Rows...>(*)(), TransitionTableFn>
    {
        using type = transition_table_list_t<TransitionTableFn>;
    };

    template<auto... Fns, const auto& V>
    struct to_transition_table_fn_list_helper<transition_table_list_t<Fns...>, V>
    {
        using type = transition_table_list_t<Fns...>;
    };

    /*
    The type of V is either:
    - a pointer to a function returning an instance of transition_table_t;
    - an instance of transition_table_list_t.
    */
    template<const auto& V>
    using to_transition_table_fn_list_t = typename to_transition_table_fn_list_helper<std::decay_t<decltype(V)>, V>::type;
}

} //namespace

#endif

//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_DETAIL_STATE_TRAITS_HPP
#define AWESM_DETAIL_STATE_TRAITS_HPP

#include "../state_conf.hpp"

namespace awesm::detail::state_traits
{

#define AWESM_DETAIL_STATE_TRAITS_REQUIRES_ON_XXX(xxx) \
    template<class... Options> \
    struct xxx##_option_pattern; \
 \
    template<class Option, class... Options> \
    struct xxx##_option_pattern<Option, Options...> \
    { \
        using type = typename xxx##_option_pattern<Options...>::type; \
    }; \
 \
    template<class TypePattern, class... Options> \
    struct xxx##_option_pattern<state_options::on_##xxx<TypePattern>, Options...> \
    { \
        using type = TypePattern; \
    }; \
 \
    template<> \
    struct xxx##_option_pattern<> \
    { \
        using type = any_of<>; \
    }; \
 \
    template<class Conf, class Event> \
    struct requires_on_##xxx##_conf; \
 \
    template<class... Options, class Event> \
    struct requires_on_##xxx##_conf<state_conf<Options...>, Event> \
    { \
        using pattern_t = typename xxx##_option_pattern<Options...>::type; \
        static constexpr auto value = pattern_t::template matches<Event>; \
    }; \
 \
    template<class State, class Event> \
    struct requires_on_##xxx \
    { \
        static constexpr auto value = requires_on_##xxx##_conf \
        < \
            typename State::conf, \
            Event \
        >::value; \
    }; \
 \
    template<class State, class Event> \
    constexpr auto requires_on_##xxx##_v = requires_on_##xxx<State, Event>::value;

AWESM_DETAIL_STATE_TRAITS_REQUIRES_ON_XXX(event)
AWESM_DETAIL_STATE_TRAITS_REQUIRES_ON_XXX(exit)

#undef AWESM_DETAIL_STATE_TRAITS_REQUIRES_ON_XXX

} //namespace

#endif

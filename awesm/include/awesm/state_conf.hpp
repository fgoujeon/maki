//Copyright Florian Goujeon 2021 - 2022.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/awesm

#ifndef AWESM_STATE_CONF_HPP
#define AWESM_STATE_CONF_HPP

#include "type_patterns.hpp"

namespace awesm
{

namespace state_options
{
    struct on_entry_any{};

    template<class TypePattern>
    struct on_event{};

    using on_event_any = on_event<any>;

    template<class... Ts>
    using on_event_any_of = on_event<any_of<Ts...>>;

    struct on_exit_any{};
}

template<class... Options>
struct state_conf: Options...
{
};

} //namespace

#endif

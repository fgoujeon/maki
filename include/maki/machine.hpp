//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

/**
@file
@brief Defines the maki::machine class template
*/

#ifndef MAKI_MACHINE_HPP
#define MAKI_MACHINE_HPP

#include "machine_conf.hpp"
#include "events.hpp"
#include "null.hpp"
#include "detail/machine_operation.hpp"
#include "detail/iseq.hpp"
#include "detail/state_impls/simple.hpp" //NOLINT misc-include-cleaner
#include "detail/state_impls/composite.hpp" //NOLINT misc-include-cleaner
#include "detail/state_impls/composite_no_context.hpp"
#include "detail/context_holder.hpp"
#include "detail/context_storage.hpp"
#include "detail/event_action.hpp"
#include "detail/noinline.hpp"
#include "detail/function_queue.hpp"
#include "detail/mix.hpp"
#include "detail/tlu/contains_if.hpp"
#include <type_traits>
#include <exception>

namespace maki
{

#define MAKI_DETAIL_MAYBE_CATCH(statements) /*NOLINT(cppcoreguidelines-macro-usage)*/ \
    if constexpr(detail::is_null_v<typename conf_type::exception_handler_type>) \
    { \
        statements \
    } \
    else \
    { \
        try \
        { \
            statements \
        } \
        catch(...) \
        { \
            detail::impl_of(conf).exception_handler(*this, std::current_exception()); \
        } \
    }

/**
@brief The state machine implementation template.
@tparam MachineConfHolder the state machine configuration, with defined `transition_tables`
and `context` options

Here is an example of valid state machine definition, where:
- `transition_table` is a user-provided `constexpr` instance of a
`maki::transition_table` type;
- `context` is a user-provided class.

@snippet concepts/state-machine/src/main.cpp machine-def

The state machine type itself can then be defined like so:
@snippet concepts/state-machine/src/main.cpp machine
*/
template<class MachineConfHolder>
class machine
{
#include "detail/aos_sync_begin.hpp"
#include "detail/machine.inc.hpp"
#include "detail/aos_end.hpp"
};

#ifdef __cpp_impl_coroutine
/**
@brief The asynchronous state machine implementation template.
@tparam MachineConfHolder the state machine configuration, with defined `transition_tables`
and `context` options
*/
template<class MachineConfHolder>
class async_machine
{
#include "detail/aos_async_begin.hpp"
#include "detail/machine.inc.hpp"
#include "detail/aos_end.hpp"
};
#endif

} //namespace

#endif

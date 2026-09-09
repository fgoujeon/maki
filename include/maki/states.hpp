//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATES_HPP
#define MAKI_STATES_HPP

#include "machine_conf.hpp"
#include "transition_table.hpp"
#include "state.hpp"
#include "detail/state_mold_ids.hpp"
#include "detail/state_impls/simple_no_context.hpp"
#include "detail/iseq.hpp"

namespace maki
{

#include "detail/aos_sync_begin.hpp"
#include "detail/states.inc.hpp"
#include "detail/aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "detail/aos_async_begin.hpp"
#include "detail/states.inc.hpp"
#include "detail/aos_end.hpp"
#endif

} //namespace

#endif

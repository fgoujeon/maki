//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_MACHINE_IMPL_HPP
#define MAKI_DETAIL_MACHINE_IMPL_HPP

#include "machine_fwd.hpp"
#include "../machine_conf.hpp"
#include "../events.hpp"
#include "../null.hpp"
#include "machine_operation.hpp"
#include "iseq.hpp"
#include "state_impls/simple.hpp" //NOLINT misc-include-cleaner
#include "state_impls/composite.hpp" //NOLINT misc-include-cleaner
#include "state_impls/composite_no_context.hpp"
#include "context_holder.hpp"
#include "context_storage.hpp"
#include "event_action.hpp"
#include "noinline.hpp"
#include "function_queue.hpp"
#include "mix.hpp"
#include "tlu/contains_if.hpp"
#include <type_traits>
#include <exception>

#include "aos_sync_begin.hpp"
#include "machine_impl.inc.hpp"
#include "aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "aos_async_begin.hpp"
#include "machine_impl.inc.hpp"
#include "aos_end.hpp"
#endif

#endif

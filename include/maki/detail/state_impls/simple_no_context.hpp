//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_NO_CONTEXT_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_NO_CONTEXT_HPP

#include "../machine_conf_tree.hpp"
#include "../type_set.hpp"
#include "../event_action.hpp"
#include "../mix.hpp"
#include "../tlu/empty.hpp"
#include "../tlu/left_fold.hpp"
#include <type_traits>

#include "../aos_sync_begin.hpp"
#include "simple_no_context.inc.hpp"
#include "../aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "../aos_async_begin.hpp"
#include "simple_no_context.inc.hpp"
#include "../aos_end.hpp"
#endif

#endif

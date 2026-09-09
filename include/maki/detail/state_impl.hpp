//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPL_HPP
#define MAKI_DETAIL_STATE_IMPL_HPP

#include "state_impls/simple_no_context_fwd.hpp"
#include "state_impls/simple_fwd.hpp"
#include "state_impls/composite_no_context_fwd.hpp"
#include "state_impls/composite_fwd.hpp"
#include "machine_conf_tree.hpp"
#include "context_storage.hpp"
#include <type_traits>

#include "aos_sync_begin.hpp"
#include "state_impl.inc.hpp"
#include "aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "aos_async_begin.hpp"
#include "state_impl.inc.hpp"
#include "aos_end.hpp"
#endif

#endif

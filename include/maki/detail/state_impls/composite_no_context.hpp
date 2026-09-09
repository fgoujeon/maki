//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_COMPOSITE_NO_CONTEXT_HPP
#define MAKI_DETAIL_COMPOSITE_NO_CONTEXT_HPP

#include "simple_no_context.hpp"
#include "../state_mold_ids.hpp"
#include "../type_set.hpp"
#include "../region_impl.hpp"
#include "../context_storage.hpp"
#include "../iseq.hpp"
#include "../mix.hpp"
#include "../friendly_impl.hpp"
#include "../machine_fwd.hpp"
#include "../tlu/apply.hpp"
#include "../tlu/left_fold.hpp"
#include "../tlu/for_each_plus.hpp"
#include "../tlu/for_each.hpp"
#include "../tlu/get.hpp"
#include "../../region.hpp"
#include "../../context.hpp"
#include <type_traits>

#include "../aos_sync_begin.hpp"
#include "composite_no_context.inc.hpp"
#include "../aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "../aos_async_begin.hpp"
#include "composite_no_context.inc.hpp"
#include "../aos_end.hpp"
#endif

#endif

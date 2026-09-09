//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP
#define MAKI_DETAIL_STATE_IMPLS_COMPOSITE_HPP

#include "composite_no_context.hpp"
#include "../machine_fwd.hpp"
#include "../machine_conf_tree.hpp"
#include "../context_holder.hpp"
#include "../context_storage.hpp"
#include "../tlu.hpp"
#include "../../state_mold.hpp"
#include "../../context.hpp"
#include <type_traits>

#include "../aos_sync_begin.hpp"
#include "composite.inc.hpp"
#include "../aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "../aos_async_begin.hpp"
#include "composite.inc.hpp"
#include "../aos_end.hpp"
#endif

#endif

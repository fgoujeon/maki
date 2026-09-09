//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_REGION_IMPL_HPP
#define MAKI_DETAIL_REGION_IMPL_HPP

#include "state_mold_ids.hpp"
#include "machine_conf_tree.hpp"
#include "type_set.hpp"
#include "transition_table_digest.hpp"
#include "transition_table_filters.hpp"
#include "context_storage.hpp"
#include "state_impl.hpp"
#include "tuple.hpp"
#include "mix.hpp"
#include "iseq.hpp"
#include "path_impl.hpp"
#include "friendly_impl.hpp"
#include "machine_fwd.hpp"
#include "../states.hpp"
#include "../action.hpp"
#include "../guard.hpp"
#include "../path.hpp"
#include "../null.hpp"
#include "../state_mold.hpp"
#include "../state.hpp"
#include "../transition_table.hpp"
#include <type_traits>

#include "aos_sync_begin.hpp"
#include "region_impl.inc.hpp"
#include "aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "aos_async_begin.hpp"
#include "region_impl.inc.hpp"
#include "aos_end.hpp"
#endif

#endif

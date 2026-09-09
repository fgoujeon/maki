//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_HPP

#include "simple_no_context.hpp"
#include "../context_holder.hpp"
#include "../context_storage.hpp"
#include "../../context.hpp"
#include <type_traits>

#include "../aos_sync_begin.hpp"
#include "simple.inc.hpp"
#include "../aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "../aos_async_begin.hpp"
#include "simple.inc.hpp"
#include "../aos_end.hpp"
#endif

#endif

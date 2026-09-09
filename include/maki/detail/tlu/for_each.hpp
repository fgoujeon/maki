//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_TLU_FOR_EACH_HPP
#define MAKI_DETAIL_TLU_FOR_EACH_HPP

#include "../co_util.hpp"
#include "../friendly_impl.hpp"
#include <type_traits>

#include "../aos_sync_begin.hpp"
#include "for_each.inc.hpp"
#include "../aos_end.hpp"

#ifdef __cpp_impl_coroutine
#include "../aos_async_begin.hpp"
#include "for_each.inc.hpp"
#include "../aos_end.hpp"
#endif

#endif

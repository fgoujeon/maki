//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#include <maki.hpp>
#include "common.hpp"

#include "aos/begin_sync.inc.hpp"
#include "basic_transition.inc.hpp"
#include "aos/end.inc.hpp"

#if MAKI_BUILD_TESTS_20
#include "aos/begin_async.inc.hpp"
#include "basic_transition.inc.hpp"
#include "aos/end.inc.hpp"
#endif

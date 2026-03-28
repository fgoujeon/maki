//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_STATE_IMPLS_SIMPLE_FWD_HPP
#define MAKI_DETAIL_STATE_IMPLS_SIMPLE_FWD_HPP

#include "../context_storage.hpp"

namespace maki::detail::state_impls
{

template<auto Id, class ContextType, context_storage ParentCtxStorage>
class simple;

template<auto Id>
using simple_no_context = simple<Id, void, context_storage::plain>;

} //namespace

#endif

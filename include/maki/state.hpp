//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_HPP
#define MAKI_REGION_STATE_HPP

#include "detail/type_set.hpp"
#include "detail/pretty_name.hpp"
#include "detail/friendly_impl.hpp"
#include "detail/tlu/left_fold.hpp"
#include <string_view>
#include <utility>

namespace maki
{

/**
@brief Represents a [state](@ref state).
@tparam Impl implementation detail

Objects of this type can only be created by Maki itself.
*/
template<class Impl>
class state
{
public:
#ifndef MAKI_DETAIL_DOXYGEN
    template<class... Args>
    constexpr state(Args&&... args):
        impl_(std::forward<Args>(args)...)
    {
    }
#endif

    state(const state&) = delete;
    state(state&&) = delete;
    state& operator=(const state&) = delete;
    state& operator=(state&&) = delete;
    ~state() = default;

    /**
    @brief Returns the `maki::state` object created by `StateMold` (of type
    `maki::state_mold`). Only valid if state is composite and only made of
    one region.
    */
    template<const auto& StateMold>
    [[nodiscard]] const auto& substate() const
    {
        return impl_.template state<StateMold>();
    }

    /**
    @brief Returns the `maki::region` object at index `Index`. Only valid if
    state is composite.
    */
    template<int Index>
    [[nodiscard]] const auto& region() const
    {
        return impl_.template region<Index>();
    }

    /**
    @brief Returns whether the state created by `StateMold` is active. Only
    valid if state is composite and only made of one region.
    */
    template<const auto& StateMold>
    [[nodiscard]] bool is() const
    {
        return impl_.template is<StateMold>();
    }

    /**
    @brief Returns the context instantiated at construction.
    */
    [[nodiscard]] const auto& context() const
    {
        return impl_.context();
    }

    /**
    @brief The pretty name of the state.

    Returns either:
    - the value given to `maki::state_mold::pretty_name()`, if any;
    - the name of the `maki::state_mold` variable, without scope nor template
    argument list.
    */
    [[nodiscard]] static std::string_view pretty_name()
    {
        return detail::pretty_name<Impl::mold>();
    }

private:
    MAKI_DETAIL_FRIENDLY_IMPL

    using impl_type = Impl;

    impl_type impl_;
};

namespace detail
{
    template<class EventTypeSet, class State>
    using state_type_list_event_type_set_operation_t = type_set_union_t
    <
        EventTypeSet,
        typename impl_of_t<State>::event_type_set
    >;

    template<class StateTypeList>
    using state_type_list_event_type_set_t = boost::mp11::mp_fold
    <
        StateTypeList,
        empty_type_set_t,
        state_type_list_event_type_set_operation_t
    >;
}

} //namespace

#endif

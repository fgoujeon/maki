//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_PROXY_HPP
#define MAKI_REGION_STATE_PROXY_HPP

#include "detail/state_proxy_fwd.hpp"
#include <functional>

namespace maki
{

namespace detail
{
    template<class State>
    state_proxy<State> make_state_proxy(const State& state)
    {
        return state_proxy<State>{state};
    }
}

/**
@brief Provides read-only access to an internal state object.
*/
#ifdef MAKI_DETAIL_DOXYGEN
template<IMPLEMENTATION_DETAIL>
#else
template<class State>
#endif
class state_proxy
{
public:
    state_proxy(const State& state):
        state_(state)
    {
    }

    state_proxy(const state_proxy&) = default;
    state_proxy(state_proxy&&) = delete;
    state_proxy& operator=(const state_proxy&) = default;
    state_proxy& operator=(state_proxy&&) = delete;
    ~state_proxy() = default;

    template<const auto& StateConf>
    [[nodiscard]] auto state() const
    {
        return detail::make_state_proxy(state_.get().template state<StateConf>());
    }

    template<int Index>
    [[nodiscard]] auto region() const
    {
        return state_.get().template region<Index>();
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return state_.get().template is<StateConf>();
    }

    [[nodiscard]] bool running() const
    {
        return state_.get().running();
    }

    [[nodiscard]] const auto& context() const
    {
        return state_.get().context();
    }

private:
    std::reference_wrapper<const State> state_;
};

} //namespace

#endif

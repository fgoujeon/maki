//Copyright Florian Goujeon 2021 - 2024.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_REGION_STATE_PROXY_HPP
#define MAKI_REGION_STATE_PROXY_HPP

namespace maki
{

template<class State>
class region_proxy;

template<class State>
class state_proxy
{
public:
    state_proxy(const State& state):
        state_(state)
    {
    }

    template<int Index>
    [[nodiscard]] auto region() const
    {
        return region_proxy{state_.template region<Index>()};
    }

    template<const auto& StateConf>
    [[nodiscard]] bool is() const
    {
        return state_.template is<StateConf>();
    }

    [[nodiscard]] bool running() const
    {
        return state_.running();
    }

private:
    const State& state_; //NOLINT cppcoreguidelines-avoid-const-or-ref-data-members
};

} //namespace

#endif

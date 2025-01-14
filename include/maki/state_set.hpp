//Copyright Florian Goujeon 2021 - 2025.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_STATE_SET_HPP
#define MAKI_STATE_SET_HPP

#include "state_conf_fwd.hpp"
#include "none.hpp"
#include "any.hpp"
#include "detail/set_predicates.hpp"

namespace maki
{

/**
@brief Represents a set of states. See @ref filter.
Use the predefined variables (`maki::any_state`, `maki::no_state`) and builder
functions (`maki::any_state_if()`, `maki::any_state_if_not()`,
`maki::any_state_of()`, `maki::any_state_but()`) instead of manually
instantiating this type.
*/
template<class Predicate>
class state_set;

namespace detail
{
    template<class Predicate>
    constexpr auto make_state_set(const Predicate& pred)
    {
        return state_set<Predicate>{pred};
    }
}

template<class Predicate>
class state_set
{
public:
    constexpr explicit state_set(const any_t& /*any*/):
        predicate_(detail::set_predicates::any{})
    {
    }

    constexpr explicit state_set(const none_t& /*any*/):
        predicate_(detail::set_predicates::none{})
    {
    }

    constexpr state_set(const state_set& other) = default;
    constexpr state_set(state_set&& other) = default;

    ~state_set() = default;

    constexpr state_set& operator=(const state_set& other) = default;
    constexpr state_set& operator=(state_set&& other) = default;

    template<class StateConfImpl>
    constexpr bool contains(const state_conf<StateConfImpl>& stt_conf) const
    {
        return predicate_(&stt_conf);
    }

private:
    template<class Predicate2>
    friend constexpr auto detail::make_state_set(const Predicate2&);

    constexpr state_set(const Predicate& pred):
        predicate_(pred)
    {
    }

    Predicate predicate_;
};

state_set(any_t) -> state_set<detail::set_predicates::any>;

state_set(none_t) -> state_set<detail::set_predicates::none>;

namespace detail
{
    template<class StateConfImpl>
    constexpr auto make_state_set_from_state_conf(const state_conf<StateConfImpl>& stt_conf)
    {
        return make_state_set(set_predicates::exactly{&stt_conf});
    }
}

template<class Impl>
constexpr auto operator!(const state_set<Impl>& stt_set)
{
    return detail::make_state_set
    (
        [stt_set](const auto pstate_conf)
        {
            return !stt_set.contains(*pstate_conf);
        }
    );
}

template<class StateConfImpl>
constexpr auto operator!(const state_conf<StateConfImpl>& stt_conf)
{
    return !detail::make_state_set_from_state_conf(stt_conf);
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator||(const state_set<LhsImpl>& lhs, const state_set<RhsImpl>& rhs)
{
    return detail::make_state_set
    (
        [lhs, rhs](const auto pstate_conf)
        {
            return lhs.contains(*pstate_conf) || rhs.contains(*pstate_conf);
        }
    );
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator||(const state_set<LhsImpl>& lhs, const state_conf<RhsImpl>& rhs)
{
    return lhs || detail::make_state_set_from_state_conf(rhs);
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator||(const state_conf<LhsImpl>& lhs, const state_set<RhsImpl>& rhs)
{
    return detail::make_state_set_from_state_conf(lhs) || rhs;
}

template<class LhsStateConfImpl, class RhsStateConfImpl>
constexpr auto operator||(const state_conf<LhsStateConfImpl>& lhs, const state_conf<RhsStateConfImpl>& rhs)
{
    return
        detail::make_state_set_from_state_conf(lhs) ||
        detail::make_state_set_from_state_conf(rhs)
    ;
}

template<class LhsImpl, class RhsImpl>
constexpr auto operator&&(const state_set<LhsImpl>& lhs, const state_set<RhsImpl>& rhs)
{
    return detail::make_state_set
    (
        [lhs, rhs](const auto pstate_conf)
        {
            return lhs.contains(*pstate_conf) && rhs.contains(*pstate_conf);
        }
    );
}

namespace detail
{
    template<class T>
    struct is_state_set
    {
        static constexpr auto value = false;
    };

    template<class Predicate>
    struct is_state_set<state_set<Predicate>>
    {
        static constexpr auto value = true;
    };

    template<class T>
    constexpr auto is_state_set_v = is_state_set<T>::value;
}

} //namespace

#endif

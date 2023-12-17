//Copyright Florian Goujeon 2021 - 2023.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

#ifndef MAKI_DETAIL_SIMPLE_STATE_HPP
#define MAKI_DETAIL_SIMPLE_STATE_HPP

#include "call_member.hpp"
#include "machine_object_holder.hpp"
#include "maybe_bool_util.hpp"
#include "tlu.hpp"
#include "../type_patterns.hpp"
#include "../null.hpp"

namespace maki::detail
{

namespace simple_state_detail
{
    template<class Event>
    struct for_event
    {
        template<class Action>
        struct takes_event
        {
            static constexpr auto value = matches_pattern_v
            <
                Event,
                typename Action::event_type_filter
            >;
        };
    };
}

/*
Implementation of a non-composite state
*/
template<const auto& Conf>
class simple_state
{
public:
    using conf_type = std::decay_t<decltype(Conf)>;
    using data_type = std::conditional_t
    <
        std::is_void_v<typename conf_type::data_type>,
        null_t,
        typename conf_type::data_type
    >;

    template<class Machine, class Context>
    simple_state(Machine& mach, Context& ctx):
        data_holder_(mach, ctx)
    {
    }

    data_type& data()
    {
        return data_holder_.get();
    }

    const data_type& data() const
    {
        return data_holder_.get();
    }

    template<class Machine, class Context, class Event>
    void call_entry_action(Machine& mach, Context& ctx, const Event& event)
    {
        call_state_action
        (
            Conf.entry_actions_,
            mach,
            ctx,
            data(),
            event
        );
    }

    template<class Machine, class Context, class Event, class... MaybeBool>
    void call_internal_action(Machine& mach, Context& ctx, const Event& event, MaybeBool&... processed)
    {
        call_state_action
        (
            Conf.internal_actions_,
            mach,
            ctx,
            data(),
            event
        );
        maybe_bool_util::set_to_true(processed...);
    }

    template<class Machine, class Context, class Event>
    void call_exit_action(Machine& mach, Context& ctx, const Event& event)
    {
        call_state_action
        (
            Conf.exit_actions_,
            mach,
            ctx,
            data(),
            event
        );
    }

    template<class Event>
    static constexpr bool has_internal_action_for_event()
    {
        struct not_found{};

        using first_matching_action_type = tlu::find_t
        <
            decltype(Conf.internal_actions_),
            simple_state_detail::for_event<Event>::template takes_event,
            not_found
        >;

        return !std::is_same_v
        <
            first_matching_action_type,
            not_found
        >;
    }

private:
    machine_object_holder<data_type> data_holder_;
};

} //namespace

#endif

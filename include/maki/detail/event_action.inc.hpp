//Copyright Florian Goujeon 2021 - 2026.
//Distributed under the Boost Software License, Version 1.0.
//(See accompanying file LICENSE or copy at
//https://www.boost.org/LICENSE_1_0.txt)
//Official repository: https://github.com/fgoujeon/maki

namespace maki::detail
{

template
<
    class R,
    auto EventActionPtr,
    class Machine,
    class Context,
    class Event,
    class... ExtraArgs
>
R MAKI_AOS_NAME(call_event_action)
(
    [[maybe_unused]] Machine& mach,
    [[maybe_unused]] Context& ctx,
    [[maybe_unused]] const Event& event,
    [[maybe_unused]] ExtraArgs&&... extra_args
)
{
    MAKI_AOS_CALL MAKI_AOS_NAME(call_callable)<R, action_signature, EventActionPtr->sig>
    (
        EventActionPtr->action,
        ctx,
        mach,
        event,
        std::forward<ExtraArgs>(extra_args)...
    );
}

template
<
    class R,
    class ActionConstantList,
    class Machine,
    class Context,
    class Event,
    class... ExtraArgs
>
R MAKI_AOS_NAME(call_matching_event_action)
(
    Machine& mach,
    Context& ctx,
    const Event& event,
    ExtraArgs&&... extra_args
)
{
    using matching_action_constant_t = tlu::find_if_t
    <
        ActionConstantList,
        event_action_traits::for_event<Event>::template has_containing_event_set
    >;

    MAKI_AOS_CALL MAKI_AOS_NAME(call_event_action)<R, matching_action_constant_t::value>
    (
        mach,
        ctx,
        event,
        std::forward<ExtraArgs>(extra_args)...
    );
}

} //namespace

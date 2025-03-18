# Pre- and Post-External-Transition Hooks {#external-transition-hook}

You can ask `maki::machine` to call hooks before or after the state machine executes a non-internal transition in a region.

These hooks are configured by calling overloads and variants of the following functions:
* `maki::machine_conf::pre_external_transition_hook_crset()`;
* `maki::machine_conf::post_external_transition_hook_crset()`.

These hooks take the following arguments:
* `ctx`, the context of the state machine;
* `region` (of type `const maki::region<...>&`), the region where the transition is happening;
* `source_state` (of type `const maki::state<...>&`), the source state;
* `event`, the event that triggered the transition;
* `target_state` (of type `const maki::state<...>&`), the target state.

These hooks are particularly useful to log transitions. For example:

@snippet extra/hook/external-transition/src/main.cpp external-transition-hooks

This is the kind of output you can obtain:

@include extra/hook/external-transition/expected-output.txt

> [!note]
> In the output above, `0` is the path to the first topmost region of the state machine.
> Also, see @ref external-transition-hook-null-state for an explanation about the empty state names.

## Note: Transition Decomposition

When composite states are involved, a unique transition can trigger several invocations of the external transition hooks. This is due to the fact that external transition hooks see transitions from the point of view of every region.

Consider the state machine below, which defines the behavior of some kind of plane:

@startuml{external_transition_hook_decomposition.png} ""
[*] --> initializing

state initializing {
    [*] --> deploying_left_wing
    deploying_left_wing --> starting_left_reactor : left_wing_sensor_signal
    starting_left_reactor --> [*] : left_reactor_rotation

    ||

    [*] --> deploying_right_wing
    deploying_right_wing --> starting_right_reactor : right_wing_sensor_signal
    starting_right_reactor --> [*] : right_reactor_rotation
}

initializing --> flying
initializing --> failing : error

hide empty description
@enduml

Whenever an `error` occurs while, say, the plane is starting its left reactor and deploying its right wing, the external transition hooks log the following messages:

@include extra/hook/external-transition-decomposition/expected-output.txt

> [!note]
> See @ref external-transition-hook-null-state for an explanation about the empty state names.

From the point of view of the topmost region (path `0`), a transition from `initializing` to `failing` occurs and triggers these subtransitions:
1. from the point of view of the first region of `initializing` (path `0/initializing/0`), a transition from `starting_left_reactor` to *nothing*;
2. from the point of view of the second region of `initializing` (path `0/initializing/1`), a transition from `deploying_right_wing` to *nothing*.

These *transitions to nothing* happen because, from the point of view of the parent region, the source state is exited but no target state is entered.

## Note: The Null Dummy State Object {#external-transition-hook-null-state}

In some cases, the `source_state` and `target_state` parameters can be given `maki::states::null`, a dummy `maki::state` object whose pretty name is an empty string.

This dummy state is used to represent:
* the source state for transitions from pseudostates (such as the initial pseudostate);
* the target state for transitions that exit the region.

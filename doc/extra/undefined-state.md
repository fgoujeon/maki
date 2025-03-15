# The Undefined State {#undefined-state}

## Definition

The **undefined state** is a predefined state that is active during external transitions. This is a safety feature that is not part of the UML specification.

## Why We Need the Undefined State

Executing an external transition mainly consists of:
1. executing the exit action of the source state;
2. executing the transition action;
3. executing the entry action of the target state.

Also, at some point, the target state must be activated (i.e. marked as the active state of the region).

Maki enforces the following guarantees for external transitions:
* The source state shall not stay active after the execution of the transition starts.
* The target state shall not be active if the execution of the transition fails.

If any of the steps above throws, leaving the source active or activating the target would break one of these guarantees. This is why we need a third option: an intermediate state that is activated before the execution of the transition and deactivated (for the benefit of the targe state) after the execution of the transition.

## How to Use the Undefined State with Maki

Maki defines these two objects:
* `maki::undefined`, the state builder of the undefined state, to be given to transition tables;
* `maki::states::undefined`, the state object created by `maki::undefined`, given to transition hooks.

While it can be relevant, you don't necessarily have to define transitions from `maki::undefined`. You already take advantage of it by having the guarantees stated above.

Note that the undefined state is contained in open-ended @ref state-set "state sets" such as `maki::all_states`.

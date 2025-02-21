# Transitions {#transition}

A transition is the fact, for a state machine, to go from a source state to a target state, as a reaction to an event.

There are several kinds of transitions. The kind of a transition is defined by a combination of three traits:

* the hierarchical position of the target state (relatively to the source state), which is either:
    * **sibling**: the target and the source belong to the same region (you're already familiar with this case);
    * **self**: the target *is* the source (see @subpage self-transition);
    * **substate**: the target is a direct substate of the source;
    * **superstate**: the target is the direct superstate of the source;
* the nature of the trigger, which is either:
    * an **event** processed by the state machine (you're already familiar with this case);
    * a **state completion** (see @subpage completion-transition);
* the locality, which is either:
    * **external**, when the source state is exited (you're already familiar with this case);
    * **local**, when the source state is not exited (see see @subpage local-transition).

Before going further, make sure you read the pages linked above.

The table below summarizes all the combinations of the three traits:

<table>
    <tr>
        <th>Position of target</th>
        <th>Locality</th>
        <th>Trigger</th>
        <th>Notation</th>
        <th>Comment</th>
    </tr>

    <tr>
        <td>Sibling</td>
        <td>External</td>
        <td>Event</td>
        <td>
            @startuml{mini_transition_event_external_sibling.png}
            state S
            S -> T : E
            hide empty description
            @enduml
        </td>
        <td></td>
    </tr>

    <tr>
        <td>Sibling</td>
        <td>External</td>
        <td>State completion</td>
        <td>
            @startuml{mini_completion_transition_to_sibling.png}
            state S
            S -> T
            hide empty description
            @enduml
        </td>
        <td></td>
    </tr>

    <tr>
        <td>Sibling</td>
        <td>Local</td>
        <td>Event</td>
        <td></td>
        <td>Irrelevant; transitions to siblings are inevitably external.</td>
    </tr>

    <tr>
        <td>Sibling</td>
        <td>Local</td>
        <td>State completion</td>
        <td></td>
        <td>Irrelevant; transitions to siblings are inevitably external.</td>
    </tr>

    <tr>
        <td>Self</td>
        <td>External</td>
        <td>Event</td>
        <td>
            @startuml{mini_external_self_transition.png}
            state S
            S -> S : E
            hide empty description
            @enduml
        </td>
        <td></td>
    </tr>

    <tr>
        <td>Self</td>
        <td>External</td>
        <td>State completion</td>
        <td>
            @startuml{mini_transition_completion_external_self.png}
            state S {
            }
            S -> S
            hide empty description
            @enduml
        </td>
        <td>For composite states only; would cause infinite loop for simple states.</td>
    </tr>

    <tr>
        <td>Self</td>
        <td>Local</td>
        <td>Event</td>
        <td>
            @startuml{mini_internal_transition.png}
            state S
            S : E / A
            hide empty description
            @enduml
        </td>
        <td>This particular case is called an internal transition.</td>
    </tr>

    <tr>
        <td>Self</td>
        <td>Local</td>
        <td>State completion</td>
        <td></td>
        <td>Irrelevant</td>
    </tr>

    <tr>
        <td>Substate</td>
        <td>External</td>
        <td>Event</td>
        <td>![](mini_transition_sub_external_event.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Substate</td>
        <td>External</td>
        <td>State completion</td>
        <td>![](mini_transition_sub_external_completion.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Substate</td>
        <td>Local</td>
        <td>Event</td>
        <td>![](mini_transition_sub_local_event.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Substate</td>
        <td>Local</td>
        <td>State completion</td>
        <td>![](mini_transition_sub_local_completion.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Superstate</td>
        <td>External</td>
        <td>Event</td>
        <td>![](mini_transition_super_external_event.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Superstate</td>
        <td>External</td>
        <td>State completion</td>
        <td>![](mini_transition_super_external_completion.png)</td>
        <td></td>
    </tr>

    <tr>
        <td>Superstate</td>
        <td>Local</td>
        <td>Event</td>
        <td></td>
        <td>Irrelevant; transitions to superstates are inevitably external.</td>
    </tr>

    <tr>
        <td>Superstate</td>
        <td>Local</td>
        <td>State completion</td>
        <td></td>
        <td>Irrelevant; transitions to superstates are inevitably external.</td>
    </tr>
</table>

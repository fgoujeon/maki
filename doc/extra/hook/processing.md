# Pre- and Post-Processing Hooks {#processing-hook}

You can ask `maki::machine` to call hooks before or after it processes specific events.

These hooks are configured by calling overloads and variants of the following functions:
* `maki::machine_conf::pre_processing_hook_v()`;
* `maki::machine_conf::post_processing_hook_mep()`.

The pre hook is useful when you need to define a reaction to an event that is independent from the active states. For example:

@snippet extra/hook/processing/src/main.cpp pre_processing_hook

The post hook can be used to handle the case where an event hasn't triggered any transition (neither external nor local). For example:

@snippet extra/hook/processing/src/main.cpp post_processing_hook

You can add as many hook as you want for different event types or @ref event-set "event type sets".

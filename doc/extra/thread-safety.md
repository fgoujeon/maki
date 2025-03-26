# Thread Safety (Not Implemented) {#thread-safety}

`maki::machine` objects are currently not thread-safe. However, as soon as thread safety is implemented, it will be enabled by default (like all other safety features).

Thread safety comes at a cost. You can already opt-out by passing `false` to `maki::machine_conf::thread_safe()`.

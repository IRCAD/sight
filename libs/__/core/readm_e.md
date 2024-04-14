# sight::core

lowest library in _sight_. it defines core classes, communication, logging and threading mechanisms. it also brings the runtime library used to discover and load modules.

## structure

here is the list of folders in this library:

- **com**: defines signals, slots, and connections.
- **jobs**: defines classes to launch jobs that can provide progress feedback.
- **log**: provides the core developer log features (spy_log), as well as a user log.
- **memory**: handles memory allocation for big data buffers, like the ones found in images and meshes.
- **mt**: defines core thread synchronizations objects (mutexes).
- **reflection**: core classes to provide type reflection in our data.
- **runtime**: defines extensions mechanism, discovers and loads modules.
- **thread**: defines worker threads, timers, and tasks.
- **tools**: defines many utility classes to manipulate types, unique identifiers, float numbers comparison, os functions, etc.

## how to use it

### cmake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> core)
```

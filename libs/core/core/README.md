 # sight::core

Lowest library in _Sight_. It defines core classes, communication, logging and threading mechanisms. It alo brings the runtime library used to discover and load modules.

## Structure

Here is the list of folders in this library:

- **com**: defines signals, slots, and connections.
- **jobs**: defines classes to launch jobs that can provide progress feedback.
- **log**: provides the core developer log features (SpyLog), as well as a user log.
- **memory**: handles memory allocation for big data buffers, like the ones found in images and meshes.
- **mt**: defines core thread synchronizations objects (mutexes).
- **reflection**: core classes to provide type reflection in our data.
- **runtime**: defines extensions mechanism, discovers and loads modules.
- **thread**: defines worker threads, timers, and tasks.
- **tools**: defines utility classes to manipulate types, unique identifiers.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> core)
```

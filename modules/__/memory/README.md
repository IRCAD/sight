# sight::module::memory

Contains services to manage memory and lock objects to prevent dump.

## Services

- **lock_dump** : performs dumps-locks on objects. They are unlocked when the service stops.
- **dump_policy** : configures dumping policy.

## How to use it

### CMake

```cmake
target_link_libraries(my_target <PUBLIC|PRIVATE> module_memory)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

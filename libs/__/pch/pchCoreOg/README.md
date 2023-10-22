# pchCoreOg

The `pchCoreOg` library is a special target used to share a precompiled header to speed-up build time. It does not contain any code.

It includes commonly used 3rd part libraries headers such as the C++ standard library and *boost*. It is very unlikely you need to explicitly use this library. It is the default precompiled header, in Debug mode, for targets compiled with the FAST_DEBUG option.

## CMake

```cmake
# Implicit usage since it is enabled by default for all targets
sight_add_target(my_target TYPE LIBRARY)
sight_add_target(my_target TYPE TEST)

# Explicit usage
sight_add_target(my_target TYPE LIBRARY PCH pchCoreOg ...)
```

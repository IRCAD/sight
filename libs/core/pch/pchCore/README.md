# pchCore

The `pchCore` library is a special target used to share a precompiled header at build time. It does not contain any code.

It includes very basic core sight headers and commonly used headers libraries such as the C++ standard library and *boost*. It is very unlikely you need to explicitly use this library. It is the default precompiled header for `LIBRARY` and `TEST` targets. If your code uses `sight::data`, prefer `pchData` or if your code uses `sight::service`, prefer `pchServices`.

## CMake

```cmake

# Explicit usage
sight_add_target(myTarget TYPE LIBRARY PCH pchCore ...)

# Implicit usage since it is enabled by default for LIBRARY and TEST targets
sight_add_target(myTarget TYPE LIBRARY) 
sight_add_target(myTarget TYPE TEST)
```

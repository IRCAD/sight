# pchService

The `pchService` library is a special target used to share a precompiled header at build time. It does not contain any code.

It includes basic sight headers from `sight::service`, which implicitly includes `sight::core`, `sight::data`, as well as headers from the C++ standard library and *boost*. This is the default PCH for modules. If you develop a library that uses `sight::service`, you can also this target library to speed-up your build, otherwise prefer `pchData` if you use `sight::data` or stick with `pchcore`, which is the default for `LIBRARY` and `TEST` targets. Note that a variant of `pchService` exists for targets that use OpenMP, because some defines need to be present in the PCH as well. Thus if you use OpenMP, please use `pchServiceOmp`.

## CMake

```cmake

sight_add_target(myTarget TYPE LIBRARY PCH pchService ...)
```

# pchData

The `pchData` library is a special target used to share a precompiled header at build time. It does not contain any code.

It includes basic sight headers from `sight::core` and `sight::data`, and implicitly, headers from the C++ standard library and *boost*. If you develop a library that uses `sight::data`, you can use this target library to speed-up your build. If you don't use `sight::data` you can keep `pchcore`, which is the default for `LIBRARY` and `TEST` targets. If you develop a module, your code probably uses `sight::service`, so prefer `pchServices` in this case.

## CMake

```cmake

sight_add_target(myTarget TYPE LIBRARY PCH pchData ...)
```

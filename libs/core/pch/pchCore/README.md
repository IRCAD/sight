# pchCore

The `pchCore` library is a special target used to share a precompiled header at build time. It does not contain any code.

It includes very basic core sight headers and commonly used headers libraries such as the standard library boost. It is very unlikely you need this library: indeed if your code uses `sight::data`, prefer `pchData` or if your code uses `sight::service`, prefer `pchServices`.

## CMake

```cmake

sight_add_target(myTarget TYPE LIBRARY PCH pchCore ...)

```

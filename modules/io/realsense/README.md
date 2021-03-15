# realsense module

The `realsense` module contains services related to the Intel Realsense cameras.

The plugin id is `::sight::module::io::realsense`.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_io_realsense
)

```
## List of available services

- `::sight::module::io::realsense::SScan`

## Service description

### sight::module::io::realsense::SScan

This service grabs the depth, the color frame, and the poincloud from a compatible device (Realsense D400 cameras).

## Other

A dedicated documentation about the available presets is present in the [README.md](./rc/README.md) located in the rc folder.

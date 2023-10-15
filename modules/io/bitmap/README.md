# sight::module::io::bitmap

This module contains services for writing bitmap related data.

## Services

- **writer**: writes a bitmap image from a `sight::data::image`.
- **reader**: reads a bitmap image and output a `sight::data::image`.

## How to use it

### CMake

```cmake
add_dependencies(myTarget
    ...
    module_io_bitmap
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
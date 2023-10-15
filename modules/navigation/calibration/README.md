# sight::module::navigation::calibration

 Contains services to do hand-Eye calibration of tracked objects (camera, atracsys).

## Services

- **tool_calibration**: performs the calibration computation based on a **sight::data::vector** of **sight::data::matrix4**. 

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_navigation_calibration ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

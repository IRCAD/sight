# sight::module::navigation::calibration

 Contains services to do hand-Eye calibration of tracked objects (camera, atracsys).

## Services

- **SToolCalibration**: performs the calibration computation based on a **sight::data::Vector** of **sight::data::Matrix4**. 

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_navigation_calibration ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

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

Please see doxygen of each service to know more about how to use it in xml configurations.

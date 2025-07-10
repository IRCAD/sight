# sight::module::navigation::optics

This module contains services performing optical tracking. 
In other words, it mainly contains services operating on `sight::data::frame_tl` or `sight::data::image`.

## CMake

```cmake

add_dependencies(my_target 
                ...
                 module_navigation_optics
)

```
## Services

- **aruco_tracker**
  Tracks multiple tags with ArUco.

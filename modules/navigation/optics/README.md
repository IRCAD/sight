# sight::module::navigation::optics

This module contains services performing optical tracking. 
In other words, it mainly contains services operating on `sight::data::FrameTL` or `sight::data::Image`.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_navigation_optics
)

```
## Services

- **SArucoTracker**
  Tracks multiple tags with ArUco.

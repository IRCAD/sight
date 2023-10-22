# navigation::openvslam

Library which handles conversions between _Sight_ and [OpenVSLAM](https://github.com/xdspacelab/openvslam) data.

## Class

- **Helper**: contains methods to convert sight data to openvslam's ones, as well as openvslam configuration reader and writter.

- **OpenvslamConfig**: defines the openvslam configuration elements. Is only used in the Helper of **sight::navigation::openvslam**.


## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> navigation_openvslam ...)
```

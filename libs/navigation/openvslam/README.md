# navigation::openvslam

Library which handles conversions between _Sight_ and [OpenVSLAM](https://github.com/xdspacelab/openvslam) data.

## Class

- **Helper**
  Contains methods to convert sight data to openvslams' ones, as well as openvslam configuration reader and writter

- **OpenvslamConfig**
   Defines the openvslam configuration elements. Is only used in the Helper of sight::navigation::openvslam


## How to use it

Include the header file `navigation/openvslam/Helper.hpp` in your source file.

All methods are static, call them directly to convert sight Camera objects to OpenVSLAM camera forward and backward, and write and load OpenVSLAM configuration.  

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> navigation_openvslam ...)
```

# sight::io::opencv

Library containing helper classes for data conversion between OpenCV and Sight.

## Classes:

- **Camera**: copies the intrinsic matrix, the image size and the distortion coefficients from a Sight camera.
- **FrameTL**: manages conversion from/to OpenCV to/from data::FrameTL buffer.
- **Image**: manages conversion from/to OpenCV to/from data::Image.
- **Matrix**: converts data::Matrix4 to opencv Mat.
- **PointList**: manages conversion from/to OpenCV cv::Point2d to/from data::Point and data::PointList.
- **Type**: manages type conversion from/to OpenCV  to/from Sight.

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> io_opencv)

```


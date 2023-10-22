 
# sight::module::filter::vision

Contains services related to computer-vision & image processing, such has background/foreground segmentation, pointcloud creation from depth images, ultrasound shape extraction, ...
This module uses **OpenCV** & **Realsense** libraries.

## Services

- **colour_image_masking**: learns a foreground and background color model and allows to segment the foreground.
- **depth_image_masking**: learns a background depth image in a mask to perform a depth difference and output the foreground.
- **optical_flow**: computes optical flow (Lukas-Kanade) to detect if camera is moving or not.
- **point_cloud_from_depth_map**: computes a point cloud from a depth map obtained from an RGBD camera. Optionally, combines it with an RGB map to get a colored point cloud.
- **transform_depth_map2mm**: applies a scale on a depth map image.
- **transform_depth_tl2mm**: applies a scale on a depth map buffer extracted from a timeline.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_filter_vision ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations. 

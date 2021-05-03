 
# sight::module::filter::vision

Contains services related to computer-vision & image processing, such has background/foreground segmentation, pointcloud creation from depth images, ultrasound shape extraction, ...
This module uses **OpenCV** & **Realsense** libraries.

## Services

- **SColourImageMasking**: learns a foreground and background color model and allows to segment the foreground.
- **SDepthImageMasking**: learns a background depth image in a mask to perform a depth difference and output the foreground.
- **SOpticalFlow**: computes optical flow (Lukas-Kanade) to detect if camera is moving or not.
- **SPointCloudFromDepthMap**: computes a point cloud from a depth map obtained from an RGBD camera. Optionally, combines it with an RGB map to get a colored point cloud.
- **STransformDepthMap2mm**: applies a scale on a depth map image.
- **STransformDepthTL2mm**: applies a scale on a depth map buffer extracted from a timeline.
- **SUltrasoundImage**: extracts the ultrasound plane and its associated information from an ultrasound video.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_filter_vision ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations. 

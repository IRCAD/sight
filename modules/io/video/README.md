# sight::module::io::video

Contains services for reading and writing video frames.

## Services

- **SFrameGrabber**: extracts video frames from a camera object (`sight::data::Camera`) into a frame timeline (`sight::data::FrameTL`) using OpenCV.
- **SFrameWriter**: saves/writes the timeline frames in files, in a folder.
- **SGrabberProxy**: allows you to select a frame grabber implementation, at runtime.
- **SVideoWriter**: saves the timeline frames in a video file.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_video ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
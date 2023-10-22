# sight::module::io::video

Contains services for reading and writing video frames.

## Services

- **frame_grabber**: extracts video frames from a camera object (`sight::data::camera`) into a frame timeline (`sight::data::frame_tl`) using OpenCV.
- **frame_writer**: saves/writes the timeline frames in files, in a folder.
- **grabberProxy**: allows you to select a frame grabber implementation, at runtime.
- **videoWriter**: saves the timeline frames in a video file.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_io_video ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
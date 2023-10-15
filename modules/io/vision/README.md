# sight::module::io::vision

Contains services for reading and writing calibration files.

## Services

- **calibration_images_writer**: writes CalibrationInfo 2D images to a folder using OpenCV.

- **calibration_info_reader**: reads calibration input datasets. Loads images of chessboards to fill a calibration info structure.

- **open_cv_reader**: reads an xml/yaml file using opencv and fill a `data::camera_set`.

- **open_cv_writer**: exports data::camera_set in xml/yaml files using openCV.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_vision ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
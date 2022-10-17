# sight::module::io::vision

Contains services for reading and writing calibration files.

## Services

- **SCalibrationImagesWriter**: writes CalibrationInfo 2D images to a folder using OpenCV.

- **SCalibrationInfoReader**: reads calibration input datasets. Loads images of chessboards to fill a calibration info structure.

- **SOpenCVReader**: reads an xml/yaml file using opencv and fill a `data::CameraSet`.

- **SOpenCVWriter**: exports data::CameraSet in xml/yaml files using openCV.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_vision ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
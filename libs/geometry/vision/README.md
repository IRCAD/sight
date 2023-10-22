# sight::geometry::vision

Contains functions related to calibration and 3d reconstruction

## Classes

- **ICalibration**: generic interface meant to define calibration services.
- **ReprojectionError**
  The ReprojectionError is used to compute reprojection error in a non-linear least squares problem.
  This class is used as functor in a ::ceres::Problem.

- **helper**
  Helper contains useful functions:
  - computes the mean error of reprojection.
  - computes pose from monocular camera.
  - computes pose from stereo camera.
  - computes a transformation matrix from tracked device attached to a passive pointing tool and its tip.
  - generates an Aruco Dictionary regarding the number of wanted marker and marker size.
  - detects a chessboard with the given dimensions in the image.

## How to use it

### CMake

```cmake

target_link_libraries(my_target <PUBLIC|PRIVATE> geometry_vision)

```



# sight::module::geometry::vision

Contains services that implement some computer vision algorithms, notably to calibrate cameras and estimate object poses.

## Services

- **chess_board_detector**: detects chessboards in images and pushes detected points to CalibrationInfo objects.
- **chessboard_reprojection**: reprojects the chessboard model onto the detected points using the estimated camera pose and calibration.
- **distortion**: distorts or undistorts an image according to camera intrinsic parameters and distortion coefficients.
- **marker_to_point**: updates a point list with the center of the marker (from a matrixTL) when the extractMarker slot is
- **open_cv_extrinsic**: computes extrinsic calibration with openCV.
- **open_cv_intrinsic**: computes intrinsic calibration with openCV.
- **pose_from2d**: computes the 3d pose of a, object using 2d points.
- **reprojection_error**: computes the mean error of reprojection between 3d object points.
- **solve_pn_p**: estimates the object pose given a set of object points, their corresponding image projections.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_geometry_vision ... )
```

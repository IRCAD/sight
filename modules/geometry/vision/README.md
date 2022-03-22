# sight::module::geometry::vision

Contains services that implement some computer vision algorithms, notably to calibrate cameras and estimate object poses.

## Services

- **SChessBoardDetector**: detects chessboards in images and pushes detected points to CalibrationInfo objects.
- **SChessboardReprojection**: reprojects the chessboard model onto the detected points using the estimated camera pose and calibration.
- **SDistortion**: distorts or undistorts an image according to camera intrinsic parameters and distortion coefficients.
- **SMarkerToPoint**: updates a point list with the center of the marker (from a matrixTL) when the extractMarker slot is
- **SOpenCVExtrinsic**: computes extrinsic calibration with openCV.
- **SOpenCVIntrinsic**: computes intrinsic calibration with openCV.
- **SPoseFrom2d**: computes the 3d pose of a, object using 2d points.
- **SReprojectionError**: computes the mean error of reprojection between 3d object points.
- **SSolvePnP**: estimates the object pose given a set of object points, their corresponding image projections.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_geometry_vision ... )
```

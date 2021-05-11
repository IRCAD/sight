# sight::module::geometry::base

Contains services to do basic geometrical operations  with `sight::data::matrix4` and `sight::data::PointList`.

## Services

- **SConcatenateMatrices**: concatenates N matrices together.
- **SDecomposeMatrix**: decomposes a matrix into a 3 by 3 rotation matrix, a 3 dimensional translation vector and a 3 by 3 scaling matrix.
- **SManagePointList**: adds point in a `sight::data::PointList`.
- **SPointListFromMatrices**: extracts translation part of multiple matrices and put it into a `sight::data::PointList`.
- **SPointToLandmarkDistance**: computes the intersection between a mesh and a line.
- **SPointToLandmarkVector**: computes the vector between a point and a target.
- **SSwitchMatrices**: switches between several matrices.
- **STargeting**: returns a `sight::data::PointList` containing the position of a sight::data::Landmarks on a CrossHair.
- **STransformLandmark**: applies a transformation matrix on a `sight::data::Landmarks`.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_geometry_base ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
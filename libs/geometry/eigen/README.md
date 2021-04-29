# sight::geometry::eigen

Contains functions to handle type conversion between `Eigen` types and `sight::data` types.

## Namespaces

- **helper**
  `sight::geometry::eigen::helper` contains a set of utility functions:

  - `Eigen` 4x4 matrices to `data::Matrix4`.
  - `Eigen` 4x4 matrix to `std::pair< ::Eigen::Vector3d, ::Eigen::Vector3d>`, a rotation-translation decomposition.
  - `std::array` to `Eigen` 4x4 matrix.
  - `std::array` to `std::pair< ::Eigen::Vector3d, ::Eigen::Vector3d>`.
  - `data::Matrix4::sptr` to `Eigen` 4x4 matrix.
 

## How to use it

### CMake

```cmake
target_link_libraries(myTarget <PUBLIC|PRIVATE> geometry_eigen)
```


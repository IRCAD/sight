# sight::module::geometry

Contains services to do basic geometrical operations  with `sight::data::matrix4` and `sight::data::point_list`.

## Services

- **concatenate_matrices**: concatenates N matrices together.
- **decompose_matrix**: decomposes a matrix into a 3 by 3 rotation matrix, a 3 dimensional translation vector and a 3 by 3 scaling matrix.
- **manage_point_list**: adds point in a `sight::data::point_list`.
- **point_list_from_matrices**: extracts translation part of multiple matrices and put it into a `sight::data::point_list`.
- **point_to_landmark_distance**: computes the intersection between a mesh and a line.
- **point_to_landmark_vector**: computes the vector between a point and a target.
- **switch_matrices**: switches between several matrices.
- **targeting**: returns a `sight::data::point_list` containing the position of a sight::data::landmarks on a CrossHair.
- **transform_landmark**: applies a transformation matrix on a `sight::data::landmarks`.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_geometry ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
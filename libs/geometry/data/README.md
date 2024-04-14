# sight::geometry::data

This library contains geometrical functions to interact with our data of `sight::data`, such as `sight::data::mesh` or `sight::data::matrix4`.

It also contains geometrical functions or algorithms using custom types such as `fw_vec3d`, `fw_matrix4x4`, `fw_plane` or `fw_line`. **Please avoid to use them in new code**. Despite most of these functions use [glm](https://github.com/g-truc/glm) internally, they have to convert from scalar data to parallel data at each call and thus they are not optimal. Please use functions from `sight::geometry::glm` if possible, or consider port functions from this library to `sight::geometry::glm`.

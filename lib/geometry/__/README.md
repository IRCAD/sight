# sight::geometry

This library contains geometrical functions or algorithms that are not present in [glm](https://github.com/g-truc/glm).
It is meant to be as efficient as possible, so all functions should only take pure `glm` types, to avoid conversion and
thus performance penalties from scalar data to parallel data.

It also contains geometrical functions or algorithms using custom types such as `plane_t`, `ray_t` or `line_t`.

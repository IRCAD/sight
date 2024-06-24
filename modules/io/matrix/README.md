# sight::module::io::matrix

Module containing services to read/write matrices from/to files.

## Services

- **matrices_reader**: reads a csv file, extracts matrices from it and pushes them into a sight::data::matrix_tl.
- **matrix_writer**: saves a timeline of matrices in a csv file.
- **matrix4_trf_reader**: reads a sight::data::matrix4 from a .trf file
- **matrix4_trf_writer**: writes a sight::data::matrix4 into a .trf file.
- **validator**: checks if a given matrix4 is valid or not as a rigid transformation matrix (homogenous and orthogonal).

## How to use it

### CMake

```cmake
add_dependencies(my_target module_io_matrix ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

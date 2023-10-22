# sight::module::io::vtk

This module contains services for reading and writing VTK related data.

## Services

- **image_reader**: reads a VTK image and converts it into a `sight::data::image`.

- **image_writer**: writes a VTK image from a `sight::data::image`.

- **image_series_reader**: reads a VTK image series and converts them into a `sight::data::image_series`.

- **image_series_writer**: writes a VTK image series file from a `sight::data::image_series`.

- **meshReader**: reads a VTK mesh and converts it into a `sight::data::mesh`.

- **meshWriter**: writes a VTK mesh from a `sight::data::mesh`.

- **model_series_reader**: reads VTK files and converts them into `ReconstructionDB` from a `sight::data::model_series`.

- **model_series_writer**: writes VTK files from `ReconstructionDB` from a `sight::data::model_series`.

- **model_series_obj_writer**: same as `model_series_writer`, but limited to `.obj` files.

- **series_set_reader**: reads VTK files and converts them into a `sight::data::series_set`.

## How to use it

### CMake

```cmake
add_dependencies(my_target
    ...
    module_io_vtk
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
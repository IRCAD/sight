# sight::module::io::vtk

This module contains services for reading and writing VTK related data.

## Services

- **SImageReader**: reads a VTK image and converts it into a `sight::data::Image`.

- **SImageWriter**: writes a VTK image from a `sight::data::Image`.

- **SImageSeriesReader**: reads a VTK image series and converts them into a `sight::data::ImageSeries`.

- **SImageSeriesWriter**: writes a VTK image series file from a `sight::data::ImageSeries`.

- **SMeshReader**: reads a VTK mesh and converts it into a `sight::data::Mesh`.

- **SMeshWriter**: writes a VTK mesh from a `sight::data::Mesh`.

- **SModelSeriesReader**: reads VTK files and converts them into `ReconstructionDB` from a `sight::data::ModelSeries`.

- **SModelSeriesWriter**: writes VTK files from `ReconstructionDB` from a `sight::data::ModelSeries`.

- **SModelSeriesObjWriter**: same as `SModelSeriesWriter`, but limited to `.obj` files.

- **SSeriesSetReader**: reads VTK files and converts them into a `sight::data::SeriesSet`.

## How to use it

### CMake

```cmake
add_dependencies(myTarget
    ...
    module_io_vtk
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
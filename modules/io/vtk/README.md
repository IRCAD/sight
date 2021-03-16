# sight::module::io::vtk

This module contains services for reading and writing VTK related data.

## Services

- **SImageReader**
Reads a VTK image into a sight::data::Image

- **SImageWriter**
Writes a VTK image from a sight::data::Image

- **SImageSeriesReader**
Reads a VTK images into a sight::data::ImageSeries

- **SImageSeriesWriter**
Writes a VTK images from a sight::data::ImageSeries

- **SMeshReader**
Reads a VTK mesh into a sight::data::Mesh

- **SMeshWriter**
Writes a VTK mesh from a sight::data::Mesh

- **SModelSeriesReader**
Reads VTK files into ReconstructionDB from a sight::data::ModelSeries

- **SModelSeriesWriter**
Writes VTK files from ReconstructionDB from a sight::data::ModelSeries

- **SModelSeriesObjWriter**
Same as SModelSeriesWriter, but limited to obj format

- **SSeriesDBReader**
Reads VTK files into a sight::data::SeriesDB

## CMake

```cmake

add_dependencies(myTarget 
    ...
    module_io_vtk
)

```
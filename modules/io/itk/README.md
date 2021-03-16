# sight::module::io::itk

This module contains services for reading and writing itk file formats.

## Services

- **inrImageReaderService**
  Reads `.inr.gz` image format.
- **InrImageWriterService**
  Writes `.inr.gz` image format.
- **JpgImageWriterService**
  Writes an image to jpg format.
- **SImageSeriesWriter**
  Writes `.inr.gz` image series format.
- **SInrSeriesDBReader**
  Reads `.inr.gz` files and pushes them into SeriesDB.
- **SJpgImageSeriesWriter**
  Writes an image series to jpg format.


## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_io_itk
)

```
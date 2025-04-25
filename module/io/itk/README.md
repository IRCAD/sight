# sight::module::io::itk

This module contains services for reading and writing itk file formats.

## Services

- **inrImageReaderService**
  Reads `.inr.gz` image format.
- **InrImageWriterService**
  Writes `.inr.gz` image format.
- **JpgImageWriterService**
  Writes an image to jpg format.
- **image_series_writer**
  Writes `.inr.gz` image series format.
- **SInrSeriesSetReader**
  Reads `.inr.gz` files and pushes them into series_set.
- **SJpgImageSeriesWriter**
  Writes an image series to jpg format.


## CMake

```cmake

add_dependencies(my_target
                ...
                 module_io_itk
)

```
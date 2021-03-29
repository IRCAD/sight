# sight::module::io::dimse

Contains services used to deal with PACS through **DIMSE** protocol.

## Services

- **SPacsConfigurationEditor**: edits a pacs configuration.

- **SPacsConfigurationInitializer**: initializes a PACS Configuration data and synchronize it with a preference key.

- **SProgressBarController**: displays a progress bar.

- **SQueryEditor**: allows to perform queries on a pacs.
  
- **SSeriesPuller**: pulls series from a PACS.
  
- **SSeriesPusher**: pushes a DICOM series to a PACS.
  
- **SSliceIndexDicomEditor**: This editor service is used to select a slice index and pull the image from the pacs if it is not available on the local computer.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dimse ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
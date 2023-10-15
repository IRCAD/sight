# sight::module::io::dimse

Contains services used to deal with PACS through **DIMSE** protocol.

## Services

- **pacs_configuration_editor**: edits a pacs configuration.

- **pacs_configuration_initializer**: initializes a PACS Configuration data and synchronize it with a preference key.

- **progress_bar_controller**: displays a progress bar.

- **query_editor**: allows to perform queries on a pacs.
  
- **series_puller**: pulls series from a PACS.
  
- **series_pusher**: pushes a DICOM series to a PACS.
  
- **slice_index_dicom_editor**: This editor service is used to select a slice index and pull the image from the pacs if it is not available on the local computer.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dimse ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
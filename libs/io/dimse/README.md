# sight::io::dimse

Library containing DIcom Message Service Element (DIMSE) classes to read DICOM series from PACS.

## Classes:

### general

- **series_enquirer**: connects to PACS server and retrieves Series with C-GET commands.
- **series_retriever**: listens to connexions requests from PACS, accepts them and once the C-STORE request is received, 
the retriever will receive the Series.

### data

- **pacs_configuration**: defines a PACS configuration.

### exceptions

- **base**: defines Base exceptions.
- **negociate_association_failure**: defines association failure exceptions.
- **network_initialization_failure**: defines network initialization failure exceptions.
- **presentation_context_missing**: defines presentation context missing exceptions.
- **request_failure**: defines request failure exceptions.
- **request_failure**: defines tag missing exceptions.

### helper
- **series**: helps converting DICOM series into other formats. 

## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> io_dimse)

```


# sight::io::dimse

Library containing DIcom Message Service Element (DIMSE) classes to read DICOM series from PACS.

## Classes:

### general

- **SeriesEnquirer**: connects to PACS server and retrieves Series with C-GET commands.
- **SeriesRetriever**: listens to connexions requests from PACS, accepts them and once the C-STORE request is received, 
the retriever will receive the Series.

### data

- **PacsConfiguration**: defines a PACS configuration.

### exceptions

- **Base**: defines Base Exceptions.
- **NegociateAssociationFailure**: defines association failure Exceptions.
- **NetworkInitializationFailure**: defines network initialization failure Exceptions.
- **PresentationContextMissing**: defines presentation context missing Exceptions.
- **RequestFailure**: defines request failure Exceptions.
- **RequestFailure**: defines tag missing Exceptions.

### helper
- **Series**: helps converting DICOM series into other formats. 

## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> io_dimse)

```


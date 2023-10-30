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

- **Base**: defines Base exceptions.
- **NegociateAssociationFailure**: defines association failure exceptions.
- **NetworkInitializationFailure**: defines network initialization failure exceptions.
- **PresentationContextMissing**: defines presentation context missing exceptions.
- **RequestFailure**: defines request failure exceptions.
- **RequestFailure**: defines tag missing exceptions.

### helper
- **Series**: helps converting DICOM series into other formats. 

## How to use it

### CMake

```cmake

target_link_libraries( my_target <PUBLIC|PRIVATE> io_dimse)

```


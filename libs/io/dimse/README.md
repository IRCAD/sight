# sight::io::dimse

Library containing classes to read DCMTK series from PACS.

## Classes:

### general

- **SeriesEnquirer**: Reads DICOM series from PACS.
- **SeriesRetriever**: Reads DICOM series from PACS.

### data
- **PacsConfiguration**: defines a PACS configuration.

### exceptions
- **Base**: defines Base Exceptions
- **NegociateAssociationFailure**: defines association failure Exceptions
- **NetworkInitializationFailure**: defines network initialization failure Exceptions
- **PresentationContextMissing**: defines presentation context missing Exceptions
- **RequestFailure**: defines request failure Exceptions
- **RequestFailure**: defines tag missing Exceptions

### helper
- **Series**: helps converting DCMTK series into other formats 

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> io_dimse)

```


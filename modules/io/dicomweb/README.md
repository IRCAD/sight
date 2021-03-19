# sight::module::io::dicomweb

 Contains services use to deal with PACS through **HTTP** protocol.
 If your looking for **DIMSE** protocol please see **sight::module::io::dimse**.

## Services

- **SQueryEditor**: performs an HTTP query on a Pacs.

- **SSeriesPuller**: pulls DICOM series from a PACS (ex: Orthanc).

- **SSeriesPusher**: pushes a DICOM series to a PACS.

- **SSliceIndexDicomPullerEditor**: used to change slice index of the DICOM series to pull.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dicomweb ...)
```

### XML

Please see doxygen of each service to know more about how to use it in xml configurations.
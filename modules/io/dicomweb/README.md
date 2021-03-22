# sight::module::io::dicomweb

 Contains services used to deal with PACS through **HTTP** protocol.
 If you are looking for **DIMSE** protocol please see **sight::module::io::dimse**.

## Services

- **SQueryEditor**: performs an HTTP query on a Pacs.

- **SSeriesPuller**: pulls DICOM series from a PACS (ex: Orthanc).

- **SSeriesPusher**: pushes a DICOM series to a PACS (ex: Orthanc).

- **SSliceIndexDicomPullerEditor**: requests to change slice index or slice view on the pulled DICOM series.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dicomweb ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
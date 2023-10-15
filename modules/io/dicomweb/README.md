# sight::module::io::dicomweb

 Contains services used to deal with PACS through **HTTP** protocol.
 If you are looking for **DIMSE** protocol please see **sight::module::io::dimse**.

## Services

- **query_editor**: performs an HTTP query on a Pacs.

- **series_puller**: pulls DICOM series from a PACS (ex: Orthanc).

- **series_pusher**: pushes a DICOM series to a PACS (ex: Orthanc).

- **slice_index_dicom_puller_editor**: requests to change slice index or slice view on the pulled DICOM series.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dicomweb ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
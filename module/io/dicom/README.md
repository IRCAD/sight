# sight::module::io::dicom

Contains services dealing with the reading and writing of DICOM (Digital Imaging and Communications in Medicine).

## Services
- **dicom_series_anonymizer**: anonymizes a DicomSeries.
- **dicom_series_converter**: converts DicomSeries from source series_set and push result (ImageSeries, ModelSeries, ...) in a target series_set.
- **dicom_series_set_reader**: reads DICOM Series.
- **dicom_series_writer**: writes DICOM Series.
- **series_set_reader**: reads image and Model Series from DICOM.
- **series_set_writer**: writes image and Model Series in DICOM format.
- **surface_segmentation_writer**: writes 3D ModelSeries.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_io_dicom ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

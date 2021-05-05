# sight::module::io::dicom

Contains services dealing with the reading and writing of DICOM (Digital Imaging and Communications in Medicine).

## Services
- **SDicomSeriesAnonymizer**: anonymizes a DicomSeries.
- **SDicomSeriesConverter**: converts DicomSeries from source SeriesDB and push result (ImageSeries, ModelSeries, ...) in a target SeriesDB.
- **SDicomSeriesDBReader**: reads DICOM Series.
- **SDicomSeriesWriter**: writes DICOM Series.
- **SSeriesDBReader**: reads Image and Model Series from DICOM.
- **SSeriesDBWriter**: writes Image and Model Series in DICOM format.
- **SSurfaceSegmentationWriter**: writes 3D ModelSeries.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_io_dicom ...)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

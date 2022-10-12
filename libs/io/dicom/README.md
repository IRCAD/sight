# sight::io::dicom

Library containing functions for reading and writing DICOM files. It also contains various "Helper" classes to handle additional processes, like anonymization.

## Classes:

### Reader / Writer

- **Reader**: New general purpose DICOM image reader. It is intended to replace all other Readers, when it will be able
              to read non image DICOM. It reads images and put them in a sight::data::SeriesSet
- **Series**: reads / writes a sight::data::Series from/to DICOM files. For Images, prefer "Reader"
- **seriesSet**: reads / writes a sight::data::SeriesSet from/to DICOM files. It uses internally Series reader / writer
- **SurfaceSegmentation**: writes a sight::data::ModelSeries to a surface segmentation in DICOM files.

### Container

@TODO: describe `container/sr` folder

- **DicomCodedAttribute**: represents a DICOM basic coded entry.
  It stores the four mains attributes : Code Value, Coding Scheme Designator, Coding Scheme Version and Code Meaning.

- **DicomInstance**: defines a DICOM SOP instance. It is useful during the whole writing process.
  This class allows to share data between module writers.

- **DicomSurface**: defines one surface mesh item in order to transform into DICOM/Sight form.

### helper
- **DicomAnonymizer**: contains helpers to anonymize dicom files on filesystem.
- **DicomCodedAttribute**: manages DICOM coded attributes.
- **DicomDataReader**: handles GDCM data reading.
- **DicomDataTools**: contains helpers to handle GDCM data.
  It implements many tag value getters/setters.
  It defines also some tools for converting GDCM data to Sight data.
- **DicomDataWriter**: contains helpers to write information into GDCM datasets.
- **DicomDir**: extracts a list of files from a dicomdir file.
- **DicomSearch**: contains helpers to search dicom files on filesystem.
- **DicomSeries**: generates/fills DicomSeries.
- **DicomSeriesAnonymizer**: contains helpers to anonymize DicomSeries.
- **DicomSeriesSetWriter**:
- **DicomSeriesWriter**: writes a DicomSeries in DICOM format.
- **Encoding**: manages encoding.
- **Fiducial**: contains helper methods about fiducials in a `data::SeriesSet` object.
- **FileWriter**: writes a DICOM file.
- **functions**: generates a random Dicom Patient ID using GDCM.
- **SegmentedPropertyRegistry**: defines a registry which links a structure type to an entry.
- **SOPClass**: contains tools (functions) to manage SOPClass.
- **StructuredReport**: contains helpers for DICOM Structured Reporting (SR).
- **tags**: parses group and element strings and return a gdcm::Tag instance

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> io_dicom)

```


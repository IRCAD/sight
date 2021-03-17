# sight::io::dicom

Library containing functions for reading and writing DICOM files. It also contains various "Helper" classes to handle additional processes, like anonymization. 

## Classes:

### Reader / Writer

- **Series**
  Reads / Writes a sight::data::Series from/to DICOM files.

- **SeriesDB**
  Reads / Writes a sight::data::SeriesDB from/to DICOM files. It uses internally Series reader / writer

- **SurfaceSegmentation**
  Writes a sight::data::ModelSeries to a surface segmentation in DICOM files.

---------------------------------------------  
@TODO: Continue with `container` and `helper`
---------------------------------------------

## How to use it

### CMake

```cmake

target_link_libraries( myTarget <PUBLIC|PRIVATE> io_dicom)

```


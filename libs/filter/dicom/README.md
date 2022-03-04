# sight::filter::dicom

Contains filters used to pre-process DICOM files before reading

## Filter classes

The interface filter class is `sight::filter::IFilter`. All filters need to implement the abstract methods defined in it, notably `apply()`, `getName()`, `getDescription()` and `getFilterType()`

### Composite
- **CTImageStorageDefaultComposite**: reads CTImageStorage SOP class.

### Custom
- **DefaultDicomFilter**: applies default filter.
- **NoFilter**: filter that doesn't apply any modification.

### Modifier
- **SliceThicknessModifier**: modifies slice thickness.

### Sorter
- **ImagePositionPatientSorter**: uses the ImagepositionPatient tag to sort the instances.
- **InstanceNumberSorter**: uses the InstanceNumber tag to sort the instances.
- **TagValueSorter**: uses a random tag to sort the instances.

### Splitter
- **AcquisitionNumberSplitter**: uses the AcquisitionNumber tag to split the instances.
- **ImageDimensionsSplitter**: uses the image dimensions to split the instances.
- **ImageOrientationSplitter**: uses the ImageOrientationPatient tag to split the instances.
- **ImagePositionPatientSplitter**: uses the ImagePositionPatient tag to split the instances
- **ImageTypeSplitter**: uses the ImageType tag to split the instances.
- **SOPClassUIDSplitter**: uses the SOPClassUID tag to split the instances.
- **TagValueInstanceRemoveSplitter**: removes instances according to tag value
- **TagValueSplitter**: uses a random tag to split the instances
- **TemporalPositionSplitter**: uses the TemporalPositionIdentifier tag to split the instances.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> filter_dicom)
```

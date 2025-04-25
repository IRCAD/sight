# sight::filter::dicom

Contains filters used to pre-process DICOM files before reading

## filter classes

The interface filter class is `sight::filter::filter`. All filters need to implement the abstract methods defined in it, notably `apply()`, `getName()`, `getDescription()` and `get_filter_type()`

### Composite
- **ct_image_storage_default_composite**: reads CTImageStorage SOP class.

### Custom
- **default_dicom_filter**: applies default filter.
- **no_filter**: filter that doesn't apply any modification.

### Modifier
- **slice_thickness_modifier**: modifies slice thickness.

### Sorter
- **image_position_patient_sorter**: uses the ImagepositionPatient tag to sort the instances.
- **instance_number_sorter**: uses the InstanceNumber tag to sort the instances.
- **tag_value_sorter**: uses a random tag to sort the instances.

### Splitter
- **acquisition_number_splitter**: uses the AcquisitionNumber tag to split the instances.
- **image_dimensions_splitter**: uses the image dimensions to split the instances.
- **image_orientation_splitter**: uses the ImageOrientationPatient tag to split the instances.
- **image_position_patient_splitter**: uses the ImagePositionPatient tag to split the instances
- **image_type_splitter**: uses the image_t tag to split the instances.
- **sop_class_uid_splitter**: uses the sop_classUID tag to split the instances.
- **tag_value_instance_remove_splitter**: removes instances according to tag value
- **tag_value_splitter**: uses a random tag to split the instances
- **temporal_position_splitter**: uses the TemporalPositionIdentifier tag to split the instances.

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> filter_dicom)
```

# sight::ui::dicom

Contains widgets and DICOM sorters and splitters that can be graphically configured.

## Files

### _widget_
- **QHexSpinBox**: widget used to display and edit an hexadecimal value.
- **QTagSelectorWidget**: widget allowing to select a DICOM tag.

### _splitter_
- **TagValueInstanceRemoveConfigurableSplitter**: removes DICOM instances that match a tag value.
- **TagValueConfigurableSplitter**: splits DICOM instances that match a tag value.

### _sorter_
- **TagValueConfigurableSorter**: sorts DICOM instances that match a tag value.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                       ui_dicom
)
```

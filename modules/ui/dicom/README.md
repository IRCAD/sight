# sight::module::ui::dicom

This module contains actions, editors and dialogs to manage DICOM images.
## Services

- **SFilterSelectionEditor**: selects and apply filters to a DICOM image.
- **SFilterSelectorDialog**: displays a dialog to select a DICOM filter.
- **SSliceIndexDicomEditor**: selects, downloads and reads a single image slice. Used for image previews.
  
## CMake

```cmake

add_dependencies(myTarget
                 ...
                 module_ui_dicom
)
```

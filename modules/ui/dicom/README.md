# sight::module::ui::dicom

This module contains actions, editors and dialogs to manage DICOM images.
## Services

- **filter_selection_editor**: selects and apply filters to a DICOM image.
- **filter_selector_dialog**: displays a dialog to select a DICOM filter.
- **slice_index_dicom_editor**: selects, downloads and reads a single image slice. Used for image previews.
  
## CMake

```cmake

add_dependencies(myTarget
                 ...
                 module_ui_dicom
)
```

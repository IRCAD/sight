# sight::module::ui::qt

This module contains many user interface services implemented with Qt. The services are sorted in different folders:
- **activity**: interactions with application activities
- **calibration**: camera properties
- **com**: communication between services
- **image**: `sight::data::image` management or display
- **metrics**: tools to measure distances or landmarks
- **model**: `sight::data::model_series` management or display
- **reconstruction**: `sight::data::reconstruction` management or display
- **series**: `sight::data::series` or `sight::data::series_set` management
- **video**: video playback interactions
- **viz**: visualization settings or properties

The main folder is reserved for other services that do not fit any of the category above.

## Services

Here is the list of uncategorised services:

- **launch_browser**: launches a browser with a url.
- **list_view**: draws a list of string elements.
- **notifier**: displays pop-up notifications in an application.
- **parameters**: generic widget to modify simple values such as booleans, integers, floats and colors.
- **preferences_configuration**: shows a dialog to configure preferences of an application.
- **selection_menu_button**: shows a menu button. The user can select one item in the menu.
- **show_about**: shows the about frame.
- **show_help**: shows the help content.
- **status**: shows a colored square (red, orange, green) representing a status.
- **style_selector**: selects a CSS style.
- **text_status**: displays and update values (int, double or string) in a `QLabel`.

### Activity

- **activity::dynamic_view**: manages tab views containing activities.
- **activity::launcher**: launches an activity according to the selected data.
- **activity::selector**: displays a selector allowing to choose between different activities.
- **activity::sequencer**: displays an activity stepper that allows to follow the workflow of an application.
- **activity::view**: displays activities in a single view.
- **activity::wizard**: selects the data required by an activity in order to create an `Activity`.

### Calibration

- **calibration_info_editor**: handles calibration points acquisition.
- **camera_config_launcher**: adds cameras to a camera series and launches configurations to calibrate them.
- **camera_information_editor**: displays the intrinsic calibration of a camera.
- **camera_set_editor**: displays the extrinsic calibration of a camera series.
- **display_calibration_info**: displays calibration images.
- **images_selector**: allows to add images to a `data::vector` from a `data::frame_tl`.
- **intrinsic_edition**: sets the intrinsic parameter information. It displays a dialog to change the resolution of your calibration, and computes the new intrinsic parameters.
- **optical_center_editor**: shows sliders to configure an intrinsic camera calibration
- **update_intrinsic_dialog**: displays a `QDialog` in which the user can change the calibration resolution which leads to an automatic computation of the new calibration parameters. By validating the user entry, the calibration will be updated.


### Com

- **signal_button**: shows a button and send a signal when it is clicked.
- **signal_shortcut**: sends a signal when the associated shortcut is activated.

### image

- **image_info**: displays image pixel information when it receives the mouse cursor coordinates.
- **image**: displays an image.
- **slice_index_position_editor**: allows to change the slice index of an image.
- **transfer_function**: editor to select a transfer function.
- **window_level**: allows to change the min/max value of windowing.

### Metrics

- **distance**: allows to show distances in a generic scene. Represented by a button.
- **landmarks**: defines a graphical editor to edit landmarks.

### Model

- **model_series_list**: editor displaying the list of the organs in a ModelSeries.
- **organ_transformation**: displays the organs list and allow an interactive selection to set the corresponding meshes in a map.

### Reconstruction

- **representation_editor**: displays a widget to change the reconstruction representation (surface, point, edge, ...).
- **organ_material_editor**: displays a widget to change the reconstruction material (colour and transparency).

### Series

-**InsertSeries**: Used as a placeholder in `module::ui::qt::editor::selector` UI to insert create and insert new series.
-**selector**: represents the `Series` in a hierarchical view (Study/Patient->Series)
-**selector_model**: represents the 	`selector` model.
-**selector**: shows information about the medical data. It allows to manipulate (select, erase, ...) studies and series.
-**viewer**: displays a preview of the selected series in the `Vector`. For the moment, it works only on a single selection.

### Video

- **camera_device_dlg**: displays a `QDialog` to choose camera device
- **camera**: allows to select the device to use. It updates the data camera identifier
- **slider**: allows to draw a slider. It is designed to be used with `frame_grabber` to browse a video.


### Viz

- **point_editor**: displays point information.
- **matrix_viewer**: defines a viewer for a `data::matrix4`.
- **snapshot_editor**: allows to snap shot a generic scene. It is represented by a button.
- **transform_editor**: regulates the position and rotation defined in a transformation matrix.




## How to use it

### CMake

```cmake
add_dependencies(my_target module_ui_qt ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in XML configurations.
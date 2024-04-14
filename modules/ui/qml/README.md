# sight::module::ui::qml

Contains base QML code to build a Sight application.

## Services

### activity

* **sequencer**
Displays an activity stepper that allows to select the activity to launch and display the current selection

* **view**
Displays activities in a single view (when a new activity is launched, it replaces the previous one).

### image

* **slice_index_position_editor**
Allows to change the slice index of an image.

### model

* **model_series_list**
Allows to manage the ModelSeries organs.

### reconstruction

* **organ_material_editor**
Displays a widget to change the reconstruction material (color and transparency).

* **representation_editor**
Displays a widget to change the reconstruction representation (surface, point, edge, ...).

## Other classes

### activity

* **ActivityLauncherManager**
Manages the launch of activities with a sequencer.

### model

* **OrganListModel**
Manages the ModelSeries organs in Qml TableView.

## rc

The rc directory contains the QML resources allowing to create an application.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_ui_qml ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

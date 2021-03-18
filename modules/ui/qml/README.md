# sight::module::ui::qml

Contains base QML code to build a Sight application.

## Services

### activity

* **SSequencer**
Displays an activity stepper that allows to select the activity to launch and display the current selection

* **SView**
Displays activities in a single view (when a new activity is launched, it replaces the previous one).

### image

* **SSliceIndexPositionEditor**
Allows to change the slice index of an image.

### model

* **SModelSeriesList**
Allows to manage the ModelSeries organs.

### reconstruction

* **SOrganMaterialEditor**
Displays a widget to change the reconstruction material (color and transparency).

* **SRepresentationEditor**
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
add_dependencies(myTarget module_ui_qml ... )
```

### XML

Please see doxygen of each service to know more about how to use it in xml configurations.

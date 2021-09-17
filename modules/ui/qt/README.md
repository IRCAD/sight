# sight::module::ui::qt

This module contains many user interface services implemented with Qt. The services are sorted in different folders:
- **activity**: interactions with application activities
- **calibration**: camera properties
- **com**: communication between services
- **image**: `sight::data::Image` management or display
- **metrics**: tools to measure distances or landmarks 
- **model**: `sight::data::ModelSeries` management or display
- **reconstruction**: `sight::data::Reconstruction` management or display 
- **series**: `sight::data::Series` or `sight::data::SeriesDB` management
- **video**: video playback interactions
- **viz**: visualization settings or properties

The main folder is reserved for other services that do not fit any of the category above.

## Services

Here is the list of uncategorized services:

- **SLaunchBrowser**: launches a browser with a url.
- **SListView**: draws a list of string elements.
- **SNotifier**: displays pop-up notifications in an application.
- **SParameters**: generic widget to modify simple values such as booleans, integers, floats and colors.
- **SPreferencesConfiguration**: shows a dialog to configure preferences of an application.
- **SSelectionMenuButton**: shows a menu button. The user can select one item in the menu.
- **SShowAbout**: shows the about frame.
- **SShowHelp**: shows the help content.
- **SStatus**: shows a colored square (red, orange, green) representing a status.
- **SStyleSelector**: selects a CSS style.
- **STextStatus**: displays and update values (int, double or string) in a `QLabel`.

### Activity

- **activity::SDynamicView**: manages tab views containing activities.
- **activity::SLauncher**: launches an activity according to the selected data.
- **activity::SSelector**: displays a selector allowing to choose between different activities.
- **activity::SSequencer**: displays an activity stepper that allows to follow the workflow of an application.
- **activity::SView**: displays activities in a single view.
- **activity::SWizard**: selects the data required by an activity in order to create an `ActivitySeries`.

### Calibration

- **SCalibrationInfoEditor**: handles calibration points acquisition.
- **SCameraConfigLauncher**: adds cameras to a camera series and launches configurations to calibrate them.
- **SCameraInformationEditor**: displays the intrinsic calibration of a camera.
- **SCameraSeriesEditor**: displays the extrinsic calibration of a camera series.
- **SDisplayCalibrationInfo**: displays calibration images.
- **SImagesSelector**: allows to add images to a `data::Vector` from a `data::FrameTL`.
- **SIntrinsicEdition**: sets the intrinsic parameter information. It displays a dialog to change the resolution of your calibration, and computes the new intrinsic parameters.
- **SOpticalCenterEditor**: shows sliders to configure an intrinsic camera calibration
- **SUpdateIntrinsicDialog**: displays a `QDialog` in which the user can change the calibration resolution which leads to an automatic computation of the new calibration parameters. By validating the user entry, the calibration will be updated.


### Com

- **SSignalButton**: shows a button and send a signal when it is clicked.
- **SSignalShortcut**: sends a signal when the associated shortcut is activated.

### Image

- **ImageInfo**: displays image pixel information when it receives the mouse cursor coordinates.
- **ImageTransparency**: allows to change image transparency.
- **SImage**: displays an image.
- **SliceIndexPositionEditor**: allows to change the slice index of an image.
- **SMultipleTF**: editor to select a transfer function pool.
- **STransferFunction**: editor to select a transfer function
- **TransferFunctionEditor**: ***deprecated in sight 22.0***. Editor to select a transfer function.
- **WindowLevel**: allows to change the min/max value of windowing.

### Metrics

- **Distance**: allows to show distances in a generic scene. Represented by a button.
- **SLandmarks**: defines a graphical editor to edit landmarks.

### Model

- **SModelSeriesList**: editor displaying the list of the organs in a ModelSeries.
- **SOrganTransformation**: displays the organs list and allow an interactive selection to set the corresponding meshes in a composite.

### Reconstruction

- **RepresentationEditor**: displays a widget to change the reconstruction representation (surface, point, edge, ...).
- **SOrganMaterialEditor**: displays a widget to change the reconstruction material (colour and transparency).

### Series

-**InsertSeries**: Used as a placeholder in `module::ui::qt::editor::SSelector` UI to insert create and insert new series.
-**Selector**: represents the `Series` in a hierarchical view (Study/Patient->Series)
-**SelectorModel**: represents the 	`Selector` model.
-**SSelector**: shows information about the medical data. It allows to manipulate (select, erase, ...) studies and series.
-**SViewer**: displays a preview of the selected series in the `Vector`. For the moment, it works only on a single selection.

### Video

- **CameraDeviceDlg**: displays a `QDialog` to choose camera device
- **SCamera**: allows to select the device to use. It updates the data camera identifier
- **SSlider**: allows to draw a slider. It is designed to be used with `SFrameGrabber` to browse a video.


### Viz

- **PointEditor**: displays point information.
- **SMatrixViewer**: defines a viewer for a `data::Matrix4`.
- **SnapshotEditor**: allows to snap shot a generic scene. It is represented by a button.
- **STransformEditor**: regulates the position and rotation defined in a transformation matrix.




## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_qt ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in XML configurations.
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
- **SSlider**: draws a slider with an integer data.
- **SStatus**: shows a colored square (red, orange, green) representing a status.
- **SStyleSelector**: selects a CSS style.
- **STextStatus**: displays and update values (int, double or string) in a QLabel.

### Activity

- **activity::SDynamicView**: manages tab views containing activities.
- **activity::SLauncher**: launches an activity according to the selected data.
- **activity::SSelector**: displays a selector allowing to choose between different activities.
- **activity::SSequencer**: displays an activity stepper that allows to follow the workflow of an application.
- **activity::SView**: displays activities in a single view.
- **activity::SWizard**: selects the data required by an activity in order to create an ActivitySeries.

### Calibration

### Com

### Image

### Metrics

### Model

### Reconstruction

### Series

### Video

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_qt ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
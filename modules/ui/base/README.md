# sight::module::ui::base

Contains basic user interface services.
Some services have specific categories (such as communication, metrics, io, series, videos and visualization)
and are, therefore, contained in sub-folders according to their specificity.

## Services
### General
* **SConfigLauncher**: starts/stops a template configuration.

* **SDefaultAction**: defines the default action to have a simple action button.

* **SFrame**: defines the default frame for standard application.

* **SJobBar**: displays a progress bar.

* **SMenu**: defines the default menu for standard application.

* **SMenuBar**: defines the default menubar for standard application.

* **SModifyLayout**: modifies the layout of a configuration.

* **SQuit**: closes the window and reset root object.

* **SToolBar**: defines the default toolbar for standard application.

* **SView**: defines the generic layout for application.

### com
* **SSignal**: sends a signal when it is triggered.

* **SStarter**: applies an action (start, stop, ...) on a service specify by uid.

* **STimestampSlotCaller**: runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.

### io
* **SFolderSelector**: raises a pop-up dialog to select a directory, then send it via a signal.

* **SSelector**: displays a list of available readers or writers and lets you select one to load or save a data.

### metrics
* **SAddDistance**: adds distances on an image field.

* **SRemoveDistance**: removes distances from the image field.
  Possibility to choose between removing a selected point or the last added point.

* **SShowDistance**: allows to show/hide image distances.

### series
* **SDBMerger**: allows to load a new SeriesSet and merge it with the current SeriesSet.

* **SExport**: allows to export the series specified in config to the SeriesSet.

* **SExportWithSeriesSet**: allows to export the current series using a temporary SeriesSet.

* **SPushSelection**: action that pushes selected series into a `sight::data::SeriesSet`.

### video
* **SImagePicker**: listens to a picking interaction and fills the clicked point list.

### viz
* **CrossTypeAction**: allows change the cross type.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_base ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

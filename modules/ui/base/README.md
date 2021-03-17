# sight::module::ui::base

Contains basic user interface services.
Some services have specific categories (such as communication, metrics, io, series, videos and visualization) 
and are, therefore, contained in sub-folders according to their specificity. 

## Services
### General (no sub-folder)
* **SConfigLauncher**
  Starts/stops a template configuration.

* **SDefaultAction**
  Defines the default action to have a simple action button.

* **SFrame**
  Defines the default frame for standard application.

* **SJobBar**
  Displays a progress bar.

* **SMenu**
  Defines the default menu for standard application.

* **SMenuBar**
  Defines the default menubar for standard application.

* **SModifyLayout**
  Modifies the layout of a configuration.

* **SPreferences**
  Loads and save preferences.

* **SPushField**
  Adds or removes an object's Field in `sight::data::Composite` with specific key.

* **SPushObject**
  To add or remove an object in composite with specific key.

* **SQuit**
  Closes the window and reset root object.

* **SToolBar**
  Defines the default toolbar for standard application.

* **SView**
  Defines the generic layout for application.

### com
* **SBooleanSlotCaller**
  Runs a slot given by HasSlot id and slot key with a boolean in parameter representing the action state (active or not).
  
* **SSignal**
  Sends a signal when it is triggered.
  
* **SSlotCaller**
  Runs a slot given by HasSlot id and slot key.
  
* **SStarter**
  Applies an action (start, stop, ...) on a service specify by uid.
  
* **STimestampSlotCaller**
  Runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.
  
### io
* **SFolderSelector**
  Raises a pop-up dialog to select a directory, then send it via a signal.
  
* **SSelector**
  Displays a list of available readers or writers and lets you select one to load or save a data.
  
### metrics
* **SAddDistance**
  Adds distances on an image field.
  
* **SRemoveDistance**
  Removes distances from the image field. 
  Possibility to choose between removing a selected point or the last added point.

* **SShowDistance**
  Allows to show/hide image distances.
  
### series
* **SDBMerger**
  Allows to load a new SeriesDB and merge it with the current SeriesDB.
  
* **SExport**
  Allows to export the series specified in config to the SeriesDB.
  
* **SExportWithSeriesDB**
  Allows to export the current series using a temporary seriesDB.
  
### video
* **SImagePicker**
  Listens to a picking interaction and fills the clicked point list.
  
### viz
* **CrossTypeAction**
  Allows change the cross type.
* **SSnapshot**
  Allows to snapshot a generic scene.
  
## How to use it

### CMake

```cmake
add_dependencies(myTarget module_ui_base ... )
```

### XML

Please see doxygen of each service to know more about how to use it in xml configurations.
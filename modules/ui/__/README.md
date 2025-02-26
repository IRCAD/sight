# sight::module::ui

Contains basic user interface services.
Some services have specific categories (such as communication, metrics, io, series, videos and visualization)
and are, therefore, contained in sub-folders according to their specificity.

## Services
### General
* **config_launcher**: starts/stops a template configuration.

* **action**: defines the default action to have a simple action button.

* **frame**: defines the default frame for standard application.

* **jobbar**: displays a progress bar.

* **menu**: defines the default menu for standard application.

* **menubar**: defines the default menubar for standard application.

* **modify_layout**: modifies the layout of a configuration.

* **quit**: closes the window and reset root object.

* **toolbar**: defines the default toolbar for standard application.

* **view**: defines the generic layout for application.

### com

* **timestamp_slot_caller**: runs a slot given by HasSlot id and slot key and set the current timestamp in parameter.

### io
* **folder_selector**: raises a pop-up dialog to select a directory, then send it via a signal.

* **selector**: displays a list of available readers or writers and lets you select one to load or save a data.

### metrics
* **SAddDistance**: adds distances on an image field.

### series
* **db_merger**: allows to load a new series_set and merge it with the current series_set.

* **export**: allows to export the series specified in config to the series_set.

* **export_with_series_set**: allows to export the current series using a temporary series_set.

* **push_selection**: action that pushes selected series into a `sight::data::series_set`.

### video
* **image_picker**: listens to a picking interaction and fills the clicked point list.

### viz
* **cross_type_action**: allows change the cross type.

## How to use it

### CMake

```cmake
add_dependencies(my_target module_ui ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.

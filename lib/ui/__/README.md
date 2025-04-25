# sight::ui

This is the core library in _sight_ which defines the various base interface for User Interface services.

## Classes

- **action_callback_base**: defines a graphical menu item with callback
- **application**: gives access to the underlayed application part
- **cursor**: defines a generic graphical cursor
- **GuiBaseObject**: base class for all graphical objects classes
- **registry**: internal mechanism: contains helpers to manage the various graphical container in a  _sight_ application.
- **IAction**: defines the generic interface for the menu items management
- **application**: defines the generic interface for application manipulation
- **cursor**: defines the generic interface to define a graphical cursor
- **dialog_editor**: defines the generic interface to define a graphical dialog editor service which creates its own container
- **editor**: defines the generic interface to define a graphical object editor services
- **frame**: defines the generic interface to define a frame graphical display
- **IGuiContainer**: defines the generic interface to manage the layouts and the various displayed object container
- **has_parameters**: generic interface meant to define basic slots to receive parameter values from signals
- **menu**: defines the generic interface to define a graphical menu
- **menu_bar**: defines the generic interface to define a graphical menu bar
- **menu_item_callback**: defines the generic interface to define a graphical menu item callback
- **toolbar**: defines the generic interface to define a graphical tool bar

### _builder_

- **widget**: generic interface meant to define graphical container builder
- **menubar**: generic interface meant to define menu bar builder
- **slideview**: generic interface meant to define slide view builder
- **toolbar**: generic interface meant to define tool bar builder

### _dialog_

- **input_base**: defines the generic graphical interface to display dialog box
- **location_base**: defines the generic graphical interface to display a file/folder selector dialog
- **logger_base**: defines the generic graphical interface to display a dialog message and a set of logs
- **message_base**: defines the generic graphical interface to display generic message box
- **notification_base**: defines the generic graphical interface to display a generic notification popup
- **input**: defines the generic graphical interface to display an input dialog for UI.
- **progress_base**: defines the generic graphical interface to display a generic progress dialog
- **pulse_progress_base**: defines the generic graphical interface to display a pulse progress dialog
- **selector_base**: defines the generic graphical interface to display a bialog box which allows the choice of an element among several
- **location**: defines the generic graphical file/folder selector dialog
- **logger**: defines the generic graphical a dialog message and a set of logs
- **message**: defines the generic graphical message box
- **notification**: defines the generic graphical notification popup
- **progress**: defines the generic graphical progress dialog
- **pulse_progress**: defines the generic graphical pulse progress dialog
- **selector**: defines the generic graphical dialog with the choice of a single element among several

### _factory_

- **new**: generic constructor for the graphical services with restrictions on objects construction

### _preferences_

Mechanism which allows to store and get user preferences and propose them from one application start to an other one.

- **preferences**: handles user preferences

### _registry_

- **action**:  defines the action registry for the graphical interface services
- **detail**: internal mechanism: provides the instances of the factory registry
- **macro**: defines the macro which registers internally the user interfaces services
- **menu**: defines a registry which handles xml definitions of a graphical menu
- **menu_bar**: defines a registry which handles xml definitions of a graphical menu bar
- **tool_bar**: defines a registry which handles xml definitions of a graphical tool bar
- **view**: defines a registry which handles xml definitions of a graphical generic view manager


### _view_

- **activity_view**: defines the base class for services displaying activity view
- **view**: defines the service interface managing a view placed in main frame.

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE>
                       ui
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::service `to learn more about its use in xml configurations.

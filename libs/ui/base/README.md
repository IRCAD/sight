# sight::ui::base

This is the core library in _sight_ which defines the various base interface for User Interface services.

## Classes

- **ActionCallbackBase**: defines a graphical menu item with callback
- **Application**: gives access to the underlayed application part
- **Cursor**: defines a generic graphical cursor
- **GuiBaseObject**: base class for all graphical objects classes
- **GuiRegistry**: internal mechanism: contains helpers to manage the various graphical container in a  _sight_ application.
- **IAction**: defines the generic interface for the menu items management
- **IApplication**: defines the generic interface for application manipulation
- **ICursor**: defines the generic interface to define a graphical cursor
- **IDialogEditor**: defines the generic interface to define a graphical dialog editor service which creates its own container
- **IEditor**: defines the generic interface to define a graphical object editor services
- **IFrame**: defines the generic interface to define a frame graphical display
- **IGuiContainer**: defines the generic interface to manage the layouts and the various displayed object container
- **IHasParameters**: generic interface meant to define basic slots to receive parameter values from signals
- **IMenu**: defines the generic interface to define a graphical menu
- **IMenuBar**: defines the generic interface to define a graphical menu bar
- **IMenuItemCallback**: defines the generic interface to define a graphical menu item callback
- **IToolBar**: defines the generic interface to define a graphical tool bar

### _builder_

- **IContainerBuilder**: generic interface meant to define graphical container builder
- **IMenuBarBuilder**: generic interface meant to define menu bar builder
- **ISlideViewBuilder**: generic interface meant to define slide view builder
- **IToolBarBuilder**: generic interface meant to define tool bar builder

### _dialog_

- **IInputDialog**: defines the generic graphical interface to display dialog box
- **ILocationDialog**: defines the generic graphical interface to display a file/folder selector dialog
- **ILoggerDialog**: defines the generic graphical interface to display a dialog message and a set of logs
- **IMessageDialog**: defines the generic graphical interface to display generic message box
- **IMultiSelectorDialog**: defines the generic graphical interface to display a dialog box with the choice of some elements among several
- **INotificationDialog**: defines the generic graphical interface to display a generic notification popup
- **InputDialog**: defines the generic graphical interface to display an input dialog for IHM.
- **IProgressDialog**: defines the generic graphical interface to display a generic progress dialog
- **IPulseProgressDialog**: defines the generic graphical interface to display a pulse progress dialog
- **ISelectorDialog**: defines the generic graphical interface to display a bialog box which allows the choice of an element among several
- **LocationDialog**: defines the generic graphical file/folder selector dialog
- **LoggerDialog**: defines the generic graphical a dialog message and a set of logs
- **MessageDialog**: defines the generic graphical message box
- **MultiSelectorDialog**: defines the generic graphical dialog with the choice of some element among several
- **NotificationDialog**: defines the generic graphical notification popup
- **ProgressDialog**: defines the generic graphical progress dialog
- **PulseProgressDialog**: defines the generic graphical pulse progress dialog
- **SelectorDialog**: defines the generic graphical dialog with the choice of a single element among several

### _factory_

- **new**: generic constructor for the graphical services with restrictions on objects construction

### _preferences_

Mechanism which allows to store and get user preferences and propose them from one application start to an other one.

- **preferences**: handles user preferences

### _registry_

- **Action**:  defines the action registry for the graphical interface services
- **detail**: internal mechanism: provides the instances of the factory registry
- **macro**: defines the macro which registers internally the user interfaces services
- **Menu**: defines a registry which handles xml definitions of a graphical menu
- **MenuBar**: defines a registry which handles xml definitions of a graphical menu bar
- **ToolBar**: defines a registry which handles xml definitions of a graphical tool bar
- **View**: defines a registry which handles xml definitions of a graphical generic view manager


### _view_

- **IActivityView**: defines the base class for services displaying activity view
- **IView**: defines the service interface managing a view placed in main frame.

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE>
                       ui_base
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::service `to learn more about its use in xml configurations.

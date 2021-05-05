# sight::ui::qml

Contains all needed interfaces, helper classes and resources to build a Qt interface.
> QML (Qt Modeling Language) is a user interface markup language, it is a declarative language(similar to CSS and JSON)
> for designing user interface-centric applications.
_Definition from : https://en.wikipedia.org/wiki/QML_ 

## Files

- **App**: defines a QML application.
- **IQmlAppManager**: defines the base class for AppManager managing Qml services.
- **IQmlEditor**: defines the base class for service editing an object associated to a qml UI.
- **QmlEngine**: allows to manage the QQmlEngine.

### _dialog_

- **InputDialog**: defines an Input dialog.
- **LocationDialog**: defines the generic file/folder dialog.
- **LoggerDialog**: allows choosing an element among several (selections).
- **MessageDialog**: defines the generic message box.
- **MultiSelectorDialog**: allows choosing an element among several (selections).
- **NotificationDialog**: defines a notification popup.
- **ProgressDialog**: allows to select an acquisition in a patient database.
- **PulseProgressDialog**: Allows to show a pulse progress bar.
- **SelectorDialog**: allows choosing an element among several (selections).
- **StandardButton**: defines an enum to setup the button mask for StandardButtons.

### _model_

- **RoleListModel**: allows the creation of a ListModel with customizable role.
- **RoleTableModel**: creates a generic table model for qml.


## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                       ui_qml
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::ui::qml `to learn more about its use in xml configurations.

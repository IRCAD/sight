# sight::ui::qt

Contains all needed interfaces, helper classes and resources to build a Qt interface.

## Files

- **ActionCallback**: defines the menu item callback.
- **App**: defines the Qt application.
- **Application**: gives access to the qml application part.
- **Cursor**: defines the generic cursor.
- **QtMainFame**: defines a qt panel used to control a VTK 2D Negatoscope view.
- **SliceSelector**: defines a qt panel used to control a VTK 2D Negatoscope view.
- **WorkerQt**: provides an implementation of core::thread::Worker using boost::asio.

### _builder_

- **ContainerBuilder**: defines the generic layout manager.
- **MenuBarBuilder**: defines the generic layout manager.
- **SlideViewBuilder**: creates a slide widget, that can be displayed over all others widgets.
- **ToolbarBuilder**: defines the generic layout manager.

### _container_

- **QtContainer**: defines the QWidget container.
- **QtMenuBarContainer**: defines the Qt menu bar container.
- **QtMenuContainer**: defines the Qt menu bar container.
- **QtMenuItemContainer**: defines the Qt menuItem bar container.
- **QtToolBarContainer**: defines the Qt toolBar bar container.

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

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                       ui_qt
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::ui::qt `to learn more about its use in xml configurations.

# sight::ui::qt

Contains all needed interfaces, helper classes and resources to build a Qt interface.

## Files

- **action_callback**: defines the menu item callback.
- **app**: defines the Qt application.
- **application**: gives access to the qml application part.
- **cursor**: defines the generic cursor.
- **QtMainFame**: defines the Qt main frame.
- **slice_selector**: defines a Qt panel used to control a VTK 2D Negatoscope view.
- **worker_qt**: provides an implementation of core::thread::Worker using boost::asio.

### _builder_

- **widget**: defines the generic layout manager.
- **menubar**: defines a menu bar.
- **slideview**: creates a slide widget, that can be displayed over all others widgets.
- **ToolbarBuilder**: defines a tool bar.

### _container_

- **widget**: defines the QWidget container.
- **menubar**: defines the Qt menu bar container.
- **menu**: defines the Qt menu container.
- **menu_item**: defines the Qt menu item bar container.
- **toolbar**: defines the Qt tool bar container.

### _dialog_

- **input**: defines an Input dialog.
- **location**: defines the generic file/folder dialog.
- **logger**: defines a dialog showing log information.
- **message**: defines the generic message box.
- **notification**: defines a notification popup.
- **progress**: allows to select an acquisition in a patient database.
- **pulse_progress**: allows to show a pulse progress bar.
- **selector**: allows choosing an element among several (selections).

### _layout_

- **cardinal**: defines the cardinal layout manager.
- **frame**: defines the frame manager.
- **line**: defines the line layout manager.
- **menubar**: defines the menu bar layout manager for the GUI.
- **menu**: defines the menu layout manager for the GUI.
- **tab**: defines the tab layout manager.
- **toolbar**: defines the toolbar layout manager for the GUI.
- **toolbox**: defines the toolbox layout manager.

### _util_

### _widget_

## How to use it

### CMake

```cmake
target_link_libraries( my_target <PUBLIC|PRIVATE> 
                       ui_qt
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::ui::qt `to learn more about its use in xml configurations.

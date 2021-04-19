# sight::ui::qt

Contains all needed interfaces, helper classes and resources to build a Qt interface.

## Files

- **ActionCallback**: defines the menu item callback.
- **App**: defines the Qt application.
- **Application**: gives access to the qml application part.
- **Cursor**: defines the generic cursor.
- **QtMainFame**: defines the Qt main frame.
- **SliceSelector**: defines a Qt panel used to control a VTK 2D Negatoscope view.
- **WorkerQt**: provides an implementation of core::thread::Worker using boost::asio.

### _builder_

- **ContainerBuilder**: defines the generic layout manager.
- **MenuBarBuilder**: defines a menu bar.
- **SlideViewBuilder**: creates a slide widget, that can be displayed over all others widgets.
- **ToolbarBuilder**: defines a tool bar.

### _container_

- **QtContainer**: defines the QWidget container.
- **QtMenuBarContainer**: defines the Qt menu bar container.
- **QtMenuContainer**: defines the Qt menu container.
- **QtMenuItemContainer**: defines the Qt menu item bar container.
- **QtToolBarContainer**: defines the Qt tool bar container.

### _dialog_

- **InputDialog**: defines an Input dialog.
- **LocationDialog**: defines the generic file/folder dialog.
- **LoggerDialog**: defines a dialog showing log information.
- **MessageDialog**: defines the generic message box.
- **MultiSelectorDialog**: allows choosing multiple elements among several selections.
- **NotificationDialog**: defines a notification popup.
- **ProgressDialog**: allows to select an acquisition in a patient database.
- **PulseProgressDialog**: allows to show a pulse progress bar.
- **SelectorDialog**: allows choosing an element among several (selections).

### _layoutManager_

- **CardinalLayoutManager**: defines the cardinal layout manager.
- **FrameLayoutManager**: defines the frame manager.
- **LineLayoutManager**: defines the line layout manager.
- **MenuBarLayoutManager**: defines the menu bar layout manager for the GUI.
- **MenuLayoutManager**: defines the menu layout manager for the GUI.
- **TabLayoutManager**: defines the tab layout manager.
- **ToolBarLayoutManager**: defines the toolbar layout manager for the GUI.
- **ToolboxLayoutManager**: defines the toolbox layout manager.

### _util_

### _widget_

## How to use it

### CMake

```cmake
target_link_libraries( myTarget <PUBLIC|PRIVATE> 
                       ui_qt
)
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of `sight::ui::qt `to learn more about its use in xml configurations.

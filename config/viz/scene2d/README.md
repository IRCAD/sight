# config::viz::scene2d

Contains xml configurations related to 2D visualization. These configurations are used by sight_viewer and can be reused in any custom application or activity, since they implement the extension `sight::app::extension::config`.

## Configurations

- **TransferFunctionWidget**
Defines a widget view that allows to tweak a transfer function and display the histogram of a image given as parameters.

- **TransferFunctionWindow**
Opens a modal window that allows to tweak a transfer function and display the histogram of a image given as parameters.

## How to use it

### CMake

```cmake
add_dependencies(my_target config_viz_scene2d ... )
```

### XML

The `TransferFunctionWidget` configuration can be included with the following code:

```xml

<service uid="..."  type="sight::app::config_launcher" >
    <config id="sight::config::viz::scene2d::TransferFunctionWidgetCfg" />
    <object name="image" uid="..." />
    <object name="currentTF" uid="..." />
    <param name="WID_PARENT" value="..." />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::image`,
- currentTF is a `sight::data::transfer_function`.

The `TransferFunctionWindow` configuration can be included with the following code:

```xml

<service uid="..."  type="sight::app::config_launcher" >
    <config id="sight::config::viz::scene2d::transfer_function_window" />
    <object name="image" uid="..." />
    <object name="currentTF" uid="..." />
    <object name="presets" uid="..." />
    <param name="WINDOW_TITLE" value="Volume TF editor" />
    <param name="WINDOW_ICON" value="path/to/icon.svg" />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::image`,
- currentTF is a `sight::data::transfer_function`.
- presets is a `sight::data::map`.

# config::viz::scene2d

Contains xml configurations related to 2D visualization. These configurations are used by SightViewer and can be reused in any custom application or activity, since they implement the extension `sight::app::extension::config`.

## Configurations

- **TransferFunctionWidget**
Defines a widget view that allows to tweak a transfer function and display the histogram of a image given as parameters.

- **TransferFunctionWindow**
Opens a modal window that allows to tweak a transfer function and display the histogram of a image given as parameters.

## How to use it

### CMake

```cmake
add_dependencies(myTarget config_viz_scene2d ... )
```

### XML

The `TransferFunctionWidget` configuration can be included with the following code:

```xml

<service uid="..."  type="sight::app::config_controller" >
    <appConfig id="sight::config::viz::scene2d::TransferFunctionWidgetCfg" />
    <parameter replace="WID_PARENT" by="..." />
    <inout group="data">
        <key name="image" uid="..." />
        <key name="currentTF" uid="..." />
    </inout>
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::image`,
- currentTF is a `sight::data::transfer_function`.

The `TransferFunctionWindow` configuration can be included with the following code:

```xml

<service uid="..."  type="sight::app::config_controller" >
    <appConfig id="sight::config::viz::scene2d::TransferFunctionWindowCfg" />
    <parameter replace="WINDOW_TITLE" by="Volume TF editor" />
    <parameter replace="WINDOW_ICON" by="path/to/icon.svg" />
    <inout group="data">
        <key name="image" uid="..." />
        <key name="currentTF" uid="..." />
        <key name="presets" uid="..." />
    </inout>
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::image`,
- currentTF is a `sight::data::transfer_function`.
- presets is a `sight::data::composite`.

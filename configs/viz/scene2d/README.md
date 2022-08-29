# config::viz::scene2d

Contains xml configurations related to 2D visualization. These configurations are used by SightViewer and can be reused in any custom application or activity, since they implement the extension `sight::service::extension::AppConfig`.

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

<service uid="..."  type="sight::service::SConfigController" >
    <appConfig id="TransferFunctionWidgetCfg" />
    <parameter replace="WID_PARENT" by="..." />
    <inout key="image" uid="..." />
    <inout key="currentTF" uid="..." />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::Image`,
- currentTF is a `sight::data::TransferFunction`.

The `TransferFunctionWindow` configuration can be included with the following code:

```xml

<service uid="..."  type="sight::service::SConfigController" >
    <appConfig id="TransferFunctionWindowCfg" />
    <parameter replace="WINDOW_TITLE" by="Volume TF editor" />
    <parameter replace="WINDOW_ICON" by="path/to/icon.svg" />
    <inout key="image" uid="..." />
    <inout key="currentTF" uid="..." />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- image is a `sight::data::Image`,
- currentTF is a `sight::data::TransferFunction`.

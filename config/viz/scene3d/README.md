# config::viz::scene3d

Contains xml configurations related to 3D visualization. These configurations are used by sight_viewer and can be reused in any custom application or activity, since they implement the extension `sight::app::extension::config`.

## Configurations

- **OgreLightManager**
Defines a simple widget view to select and tweak lights in a 3D scene.

- **OgreOrganManager**
Defines a widget view to select reconstructions in a `sight::data::model_series` and modify shading settings such as
color and transparency.

## How to use it

### CMake

```cmake
add_dependencies(my_target config_viz_scene3d ... )
```

### XML

The `OgreLightManager` configuration can be included with the following code:

```xml

<service uid="lightManagerCfg"  type="sight::app::config_launcher" >
    <properties config="OgreLightManager" />
    <parameter replace="WID_PARENT" by="parentView" />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view.

Last, the `OgreOrganManager` configuration can be included with the following code:

```xml

<service uid="organManagerCfg"  type="sight::app::config_launcher" >
    <properties config="OgreOrganManager" />
    <parameter replace="WID_PARENT" by="parentView" />
    <parameter replace="enable_delete" by="true" />
    <inout group="object">
        <key name="organManagerModelSeries" uid="modelSeries" />
    </inout>
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- enable_delete is a boolean to enable the possibility to delete model series with the widget,
- organManagerModelSeries is a `sight::data::model_series`.

# config::viz::scene3d

Contains xml configurations related to 3D visualization. These configurations are used by SightViewer and can be reused in any custom application or activity, since they implement the extension `sight::service::extension::AppConfig`.

## Configurations

- **OgreLightManager**
Defines a simple widget view to select and tweak lights in a 3D scene.

- **OgreOrganManager**
Defines a widget view to select reconstructions in a `sight::data::ModelSeries` and modify shading settings such as
color and transparency.

## How to use it

### CMake

```cmake
add_dependencies(myTarget config_viz_scene3d ... )
```

### XML

The `OgreLightManager` configuration can be included with the following code:

```xml

<service uid="lightManagerCfg"  type="sight::service::SConfigController" >
    <appConfig id="OgreLightManager" />
    <parameter replace="WID_PARENT" by="parentView" />
</service>
```

where:
- WID_PARENT is the identifier of the parent window view.

Last, the `OgreOrganManager` configuration can be included with the following code:

```xml

<service uid="organManagerCfg"  type="sight::service::SConfigController" >
    <appConfig id="OgreOrganManager" />
    <parameter replace="WID_PARENT" by="parentView" />
    <parameter replace="MODELSERIESLIST_ENABLE_DELETE_CONFIG" by="true" />
    <inout group="data">
        <key name="organManagerModelSeries" uid="modelSeries" />
    </inout>
</service>
```

where:
- WID_PARENT is the identifier of the parent window view,
- MODELSERIESLIST_ENABLE_DELETE_CONFIG is a boolean to enable the possibility to delete model series with the widget,
- organManagerModelSeries is a `sight::data::ModelSeries`.

# sight::modules::activity

Contains helpers and services allowing to launch activities. Initializes Activity extension point registry.

## Classes

### validator/CameraSeries
- **MonoCamera**: checks if the current CameraSeries contains one and only one Camera and is calibrated.
- **StereoCamera**: checks if the current CameraSeries contains two calibrated cameras and an extrinsic matrix.

### validator/CameraSeries
- **ContainOneSkin**: checks that the ModelSeries contains one and only one Skin.
- **ContainOneTool**: checks that the ModelSeries contains one and only one Tool.
-
## How to use it

### CMake

```cmake
add_dependencies(myTarget module_activity ... )
```

### XML

example of activity factory:

 ```xml
<extension implements="sight::activity::extension::Activity">
    <id>SDBVRRender</id>
    <title>SDB</title>
    <desc>Series Selector</desc>
    <icon>sight::module::ui::icons/selector.svg</icon>
    <requirements />
    <appConfig id="SDBVRRender">
        <parameters />
    </appConfig>
</extension>
 ```
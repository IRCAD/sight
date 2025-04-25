# sight::modules::activity

Contains helpers and services allowing to launch activities. Initializes Activity extension point registry.

## Classes

### validator/CameraSet
- **mono_camera**: checks if the current CameraSet contains one and only one Camera and is calibrated.
- **stereo_camera**: checks if the current CameraSet contains two calibrated cameras and an extrinsic matrix.

### validator/ModelSeries
- **contain_one_skin**: checks that the ModelSeries contains one and only one Skin.
- **contain_one_tool**: checks that the ModelSeries contains one and only one Tool.
-
## How to use it

### CMake

```cmake
add_dependencies(my_target module_activity ... )
```

### XML

example of activity factory:

 ```xml
<extension implements="sight::activity::extension">
    <id>myActivity</id>
    <title>SDB</title>
    <desc>Series selector</desc>
    <icon>sight::module::ui::icons/selector.svg</icon>
    <requirements />
    <properties config="myActivity">
        <parameters />
    </appConfig>
</extension>
 ```
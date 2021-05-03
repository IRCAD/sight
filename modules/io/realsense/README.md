# sight::module::io::realsense

The `realsense` module contains services to bring support for the Intel Realsense cameras.

## Services

- `sight::module::io::realsense::SScan`

> This service grabs the depth, the color frame, and the point cloud from a compatible device (Realsense D400).


## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_io_realsense
)
```

## RealSense Presets

For a complete documentation of the realsense presets please visit [this page](https://github.com/IntelRealSense/librealsense/wiki/D400-Series-Visual-Presets).

### How-to add a new preset

#### 1. Generate a preset.json

The simplest way is to use the [realsense-viewer application](https://github.com/IntelRealSense/librealsense/tree/master/tools/realsense-viewer) and save your preset as a .json file.


#### 2. Auto-load presets in videoRealSense module

You need first copy the json file in the _rc/presets_ folder.
To be loaded correclty the preset file should have a name like "NAMEPreset.json".

The service parses the presets folder and generates a map with [Name, path].
ex HandPreset.json will give: ["Hand", "rc/presets/HandPreset.json"].

#### 3. Use the preset in videoRealsense::SScan

You can achieve this in two different ways:

1. In the xml configuration of the service by using the `preset` key. The name should be the _generated name_ i.e. **without** the "...Preset.json".

```xml
<service uid="videoGrabber" type ="sight::module::io::realsense::SScan" autoConnect="no">
    <inout key="depthTL" uid="..." />
    <inout key="frameTL" uid="..." />
    <out key="pointcloud" uid="..." />
    <inout key="cameraSeries" uid="..." />
    <config preset="NAME" />
</service>
   ```


2. By using a SParameter (see **ExRealSense** example for a real-case use)

```xml
<service uid="presets" type="sight::module::ui::qt::SParameters">
    <parameters>
        <param type="enum" name="Realsense presets" key="preset" defaultValue="Default" values="Default,HighResHighAccuracy, HighResHighDensity,HighResMidDensity,MidResHighAccuracy, MidResHighDensity,MidResMidDensity,LowResHighAccuracy, LowResHighDensity,LowResMidDensity,Hand,ShortRange,BodyScan,RemoveIR" />
    </parameters>
</service>          
```






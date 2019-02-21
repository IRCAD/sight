### RealSense Presets

For a complete documentation of the realsense presets please visit [this page](https://github.com/IntelRealSense/librealsense/wiki/D400-Series-Visual-Presets).

### How-to add a new preset

#### 1. Generate a preset.json
The simpliest way is to use the [realsense-viewer application](https://github.com/IntelRealSense/librealsense/tree/master/tools/realsense-viewer) and save your preset as a .json file.


#### 2. Auto-load presets in videoRealSense Bundle

You need first copy the json file in the _rc/presets_ folder.
To be loaded correclty the preset file should have a name like "NAMEPreset.json".

The service will parse the presets folder and generate a map with [Name, path].
ex HandPreset.json will give: ["Hand", "rc/presets/HandPreset.json"].

#### 3. Use the preset in videoRealsense::SScan

You can do this by two ways:

1. in the xml configuration of the service by using the `preset` key. The name should be the _generated name_ aka **without** the "...Preset.json".

```xml
   <service uid="videoGrabber" type ="::videoRealSense::SScan" autoConnect="no">
        <inout key="depthTL" uid="..." />
        <inout key="frameTL" uid="..." />
        <out key="pointcloud" uid="..." />
        <inout key="cameraSeries" uid="..." />
        <config preset="NAME" />
   </service>
   ```


2. By using a SParameter

See **ExRealSense**

```xml
            <service uid="presets" type="::guiQt::editor::SParameters">
                <parameters>
                    <param type="enum" name="Realsense presets" key="preset" defaultValue="Default" values="Default,HighResHighAccuracy, HighResHighDensity,HighResMidDensity,MidResHighAccuracy, MidResHighDensity,MidResMidDensity,LowResHighAccuracy, LowResHighDensity,LowResMidDensity,Hand,ShortRange,BodyScan,RemoveIR" />
                </parameters>
            </service>          
```






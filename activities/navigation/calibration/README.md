# activity::navigation::calibration

Contains xml activities related to calibration (camera & tools).
Optical calibration is performed using a checkerboard.
Tool calibration is performed using a tracked aruco tag.

> **Note:** Tool calibration is deprecated and will be removed in next versions.

## Activities

- **calExtrinsicView**
Defines required configuration and service for extrinsic calibration using standard checkboard.

- **calibration**
Defines base configuration for standard calibration activity.

- **calibrationEdition**
Defines configuration for calibration edition activity.

- **calibrationEdition**
Defines configuration for calibration edition activity.

- **calIntrinsicView**
Defines configuration for the standard intrinsic camera calibration.

- **displayImageConfig**
Used to display an image and the detected points of the checkerboard.

- **displayTwoImageConfig**
Used to display two images side-by-side and the detected points of checkerboard.

- **toolCalibration**
Defines configuration needed to perform a tool calibration using the "fixed point" method (defines an half-sphere surrounding the tool by a motion around a fixed point and computes the radius) **deprecated**

- **videoEdition**
Defines base configuration for video playing.

## How to use it

### CMake

```cmake
add_dependencies(my_target activity_navigation_calibration ... )
```

### XML

Example with `calIntrinsicView` & `calExtrinsicView`

```xml
    <service uid="cameraLauncherAct" type="sight::module::ui::qt::calibration::camera_config_launcher">
        <inout key="cameraSet" uid="${cameraSet}" />
        <inout key="activity" uid="${AS_UID}" />
        <config>
            <intrinsic>
                <appConfig id="calIntrinsicView" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </intrinsic>
            <extrinsic>
                <appConfig id="calExtrinsicView" />
                <inout key="cameraSet" uid="${cameraSet}" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </extrinsic>
        </config>
    </service>
```
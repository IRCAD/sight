# activity::navigation::calibration

Contains xml activities related to calibration (camera & tools).
Optical calibration is performed using a checkerboard or charuco tags.
Tool calibration is performed using a tracked aruco tag.
   
> **Note:** Tool calibration is deprecated and will be removed in next versions.

## Activities

- **calExtrinsicCharucoView**
Defines required configuration and service for extrinsic calibration using charuco board.

- **calExtrinsicView**
Defines required configuration and service for extrinsic calibration using standard checkboard.

- **calibration**
Defines base configuration for standard calibration activity.

- **calibrationCharuco**
Defines base configuration for charuco calibration activity.

- **calibrationEdition**
Defines configuration for calibration edition activity.

- **calibrationEdition**
Defines configuration for calibration edition activity.

- **calIntrinsicCharucoView**
Defines configuration for the intrinsic camera calibration using Charuco board.

- **calIntrinsicView**
Defines configuration for the standard intrinsic camera calibration.

- **displayImageConfig**
Used to display an image and the detected points of checkboard or Charuco board.

- **displayTwoImageConfig**
Used to display two image side-by-side and the detected points of checkerboard or Charuco board.

- **toolCalibration**
Defines configuration needed to perform a tool calibration using the "fixed point" method (defines an half-sphere surrounding the tool by a motion around a fixed point and computes the radius) **deprecated**

- **videoEdition**
Defines base configuration for video playing.

## How to use it

### CMake

```cmake
add_dependencies(myTarget activity_navigation_calibration ... )
```

### XML

Example with `calIntrinsicView` & `calExtrinsicView`

```xml
    <service uid="cameraLauncherAct" type="sight::module::ui::qt::calibration::SCameraConfigLauncher">
        <inout key="cameraSeries" uid="${cameraSeries}" />
        <inout key="activitySeries" uid="${AS_UID}" />
        <config>
            <intrinsic>
                <appConfig id="calIntrinsicView" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </intrinsic>
            <extrinsic>
                <appConfig id="calExtrinsicView" />
                <inout key="cameraSeries" uid="${cameraSeries}" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </extrinsic>
        </config>
    </service>
```
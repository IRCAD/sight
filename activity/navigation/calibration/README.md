# activity::geometry::vision

Contains xml activities related to calibration (camera & tools).
Optical calibration is performed using a checkerboard.
Tool calibration is performed using a tracked aruco tag.

> **Note:** Tool calibration is deprecated and will be removed in next versions.

## Activities

- **sight::geometry::vision::cal_extrinsic_view**
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

- **videoEdition**
Defines base configuration for video playing.

## How to use it

### CMake

```cmake
add_dependencies(my_target activity_navigation_calibration ... )
```

### XML

Example with `sight::geometry::vision::cal_intrinsic_view` & `sight::geometry::vision::cal_extrinsic_view`

```xml
    <service uid="camera_launcher_act" type="sight::module::ui::qt::calibration::camera_config_launcher">
        <inout key="camera_set" uid="${camera_set}" />
        <inout key="activity" uid="${AS_UID}" />
        <config>
            <intrinsic>
                <config id="sight::geometry::vision::cal_intrinsic_view" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </intrinsic>
            <extrinsic>
                <config id="sight::geometry::vision::cal_extrinsic_view" />
                <inout key="camera_set" uid="${camera_set}" />
                <parameter replace="WID_PARENT" by="calibrationView" />
                <parameter replace="preferencesModifiedProxy" by="preferencesModifiedProxy" />
            </extrinsic>
        </config>
    </service>
```
# sight 19.0.0

## Bug fixes:

### qml

*Use of a QML Engine to recreate dialog and apply a new style.*

* fwQt has been change to add a callback function that is called to create the application using QApplication or QGuiApplication.

* created libraries:
  * guiQml: start all services needed for a Qml Application and contain element to customise Material style.
  * fwGuiQml: contain all visual Component for Qml, like dialogs.

* guiQml
  * Plugin: call getQtWorker to setup services and launch the Application. Then set the style to Material.
  * Material.qml: Singleton to set the color of the theme globally in each Window.
  * qmldir: regroup all Control.qml in rc to override the theme from QtQuick.Controls and QtQuick.Controls.Material

* fwGuiQml
  * model: regroup ListModel and TableModel that are generic model to use inside Qml
  * dialog: recreate all Qt dialog in Qml

### vlc

*Conan package depends on testing dependencies.*

Update VLC to 3.0.6-r2 to depends from Qt and FFMpeg of the testing branch.

### fwServicesTest

*Increase timeout in fwTestWaitMacro.*

- The timeout of `fwTestWaitMacro` was increased to give more time when the runner is heavily loaded.
- An optional parameter to specify the duration of the timeout was added. This is useful when we want for instance a shorter duration when we want to verify that a condition did not occurred.

### SWriter

*Automatically parse file name when saving an activity.*

* Extension is added automatically if no extension is provided in filename
* if an extension is provided in filename it will overwrite extension selected in dialog, a error pop if extension is unknown.

### SHybridMarkerTracker

*Update hybrid_marker_track to version 1.1 to improve tracking speed.*

### fwRenderOgre

*Use std::isinf() instead of isinf() from math.h.*

### fwRenderOgre

*Remove Courier.ttf font file, not allowed for redistribution.*

The font `Courier.ttf` was removed from `fwRenderOgre` and replaced by `DejaVuSans.ttf`, which was already present in the bundle `material`. `Courier.ttf` was only used for the unit-tests, so the test was modified to reflect this change.

### VRRender

*Activities creation crash.*

Add parameters `APP_NAME` and `PROGRESS_CHANNEL` in `SActivityLauncher` service in VRRender `sdb.xml`.
Modify `SActivityWizard` to check if no tab was created because the selected activity did not have any input parameters.

### ExSolvePnP

*Crash due to wrong data inout key name.*

This MR fixes the issue Sight/sight#354. It edits the INOUT key name for the meshReader service in ExSolvePnP

### calibration

*Add missing locks in SSolvePnP and SChessBoardDetector.*

Without these locks, it is possible to find a configuration where the objects are used in the same time and the application crashed.

### CMake

*Set policy CMP0072 to NEW to avoid warning about findOpenGL on Linux.*

Please refer to `cmake --help-policy CMP0072` for details

### VideoRealSense

*Only set the output when the first frame has been grabbed.*

 -  Wait until the first frame hase been grabbed to set the output. Doing so, prevent the use of an allocated but randomly filled buffer to be processed by other services, thus making them crash. See #333 for detail.

 -  Additionally, the pause mode has been fixed to not consume all cpu power.

### VRRender

*Crash at exit.*

This prevents VRRender to crash when exiting in all DICOM related activities.

Actually the crash occurred in the ioDicom plugin destruction. The usual linking hack, preventing the linker to strip symbols, consists in instancing one of the class of the bundle. It was placed in the destructor which is a bad idea because it creates an allocation of an UUID while the application is being destroyed.

### SSolvePnP

*Add missing locks.*

Without these locks, it is possible to find a configuration where the objects are used in the same time and the application crashed.

### TutoEditorQml

*Fix the crash when the application is launched.*

- The `startService` in `onServiceCreated()` is removed. `AppManager` base class has been updated to automatically start the services when they are added (if `startServices()` has been called once).

### core

*Add missing readlock.*

Some read locks are missing in central services like SCopy. Adding them may lead to slower but better, safer code

### SShaderParameter

*Force parameter update when the update slot is called.*

  -  Reset the dirty flag when calling the update. Not doing so prevented textures from being updated when modified.

### cmake

*Check CMAKE_BUILD_TYPE value.*

* on Windows, if the user doesn't specify a value for 'CMAKE_BUILD_TYPE', it's automatically initialized to 'Debug' after 'project()' cmake command.
* it's annoying because if we wanted to build in release, we have to clean the cmake cache and define `CMAKE_BUILD_TYPE` to `Release` (just redefining `CMAKE_BUILD_TYPE` isn't enough and is "dangerous")
* so now, we check `CMAKE_BUILD_TYPE` before cmake command 'project()', and if the user forgets to define `CMAKE_BUILD_TYPE` before configuring, configuration is stopped and display an error message

### RayTracingVolumeRenderer

*Allow to work with derived classes.*

### visuOgreAdaptor

*Missing color buffer when rendering point billboards.*

Add a new material to handle the case when the rendered mesh has no color buffer

### SFrameUpdater

*Change image dimension to 2D.*

Modify image creation in SFrameUpdater to change dimension from 3D to 2D

### visuOgreQt

*Fix crash in ogreResize().*

  -  Add a test to check if the ogre render windows is initialized before using it in the `ogreResize()` method

### visuOgreAdaptor

*Properly release resources.*

Releases Ogre texture properly

### ogre

*Fix infinite loop and graphic corruption in ogre shader code.*

-  Fix infinite loop and graphic corruption in ogre shader code by replacing a 'for' loop with it's 'reverse' equivalent. It is über strange, and looks like a glsl compiler bug, but the workaround seems to work.

-  Some missing `makeCurrent()` were added

### visuOgreQt

*Fix hidpi mode for various mouse event.*

-  Computes correctly the mouse and window coordinate by taking into account HiDPI (retina) display, on each events, instead of only once. This should fix problems when switching display with a different resolution. Still a manual resize may be needed to force relayouting and thus to have correct size computation.

### SGrabberProxy

*Remove spaces when parsing camera Type Tags.*

### fwRenderOgre

*Prevent alpha from leaking onto the rendering window.*

* Disable alpha writing when blending layers with the background.
* Fix the transparent widget bug.

### SAxis

*Fix the SAxis node visibility.*

`SAxis` visibility is defined by changing the visibility of its node, which can be used by other adapters. The visibility is now sets with another method.

### fwVTKQml

*Fix QML Tutos with VTK Scene.*

The vtk OpenGLRenderWindow was changed from an external to a generic one which handle his own OpenGL context. So in each render at the beginning we init the state of the OpenGL by calling OpenGLInitState. QVTKOpenGLWidget was changed by QVTKOpenGLNativeWidget because in the code source of VTK it was written: `QVTKOpenGLNativeWidget is intended to be a replacement for QVTKWidget when using Qt 5`.

### fwCore

*Fix HisResClockTest::getTimeTest.*

We use now the `std::chrono::high_resolution_clock::now()` instead of `std::chrono::system_clock::now()`.
Although it may be different, especially on macos where system_clock tends to go too fast (!!!), it seems that on most platform it is indeed the same. This need to be validated on a long test period, and may require a revert..

### OgreVolumeRendering

*Pepper artifacts when the clipping box exceeds the image size.*

Clamp clipping box coordinates to not exceed the volume size to fix some artifacts when the user sets a clipping box bigger than the rendered image.

### fwServices

*Crash when swapping the inputs of stopped services.*

### Qml

*Set conan Qt/Qml directory in QML_IMPORT_PATH".*

Allow to use QtCreator to edit Qml files with the qt package from conan.

### ARCalibration

*Reverse the model pointlist coordinates.*

Reverse x and y coordinates on the calibration model according to openCV tutorial (https://docs.opencv.org/3.4/d4/d94/tutorial_camera_calibration.html). It does not seem to change the result.

### AppManager

*Fix auto-start services in AppManager.*

Fixes `AppManager` to start automatically services when all associated objects are available.

### OgreVolumeRendering

*Fix proxy geometry generation.*

A bug occurs when the volume rendering display a tiny image (like a 10\*10).

### visuOgreAdaptor

*Fix camera ratio in Ogre SFrustumList.*

Camera ratio was wrong in `visuOgreAdaptor`::SFrustumList``because it's not the same than `visuOgreAdaptor::SFrustum`.

Now, Both adaptors have the same behavior and display the frustum at the same size and the same place.

### videoVLC

*Vlc SFrameGrabber continously pushing while paused.*

### AppConfigTest

*Add a missing wait condition in startStopTest.*

### fwDataTest

*Fix MTLockTest random failure.*

The test was not written correctly. This could lead to race conditions. We rewrote it in the following way.

The test try to lock the data to write "lili" or "tata" in a `fwData::String`. It launches two asynchronous methods that lock the data to write, wait 2 ms between each char insertion and wait 5ms after the unlock. Then, it ensures that the letters from "tata" and "lili" are not mixed in the string.

### ObjectService

*Add missing mutex.*

There was a missing mutex lock in ObjectService::unregisterServiceOutput().
This should solves the random fail in AppConfigTest::startStopTest(), and possibly other random failures in the rest of the tests.

### ARCalibration

*Fix extrinsic calibration synchronisation.*

Now, we ensure that the chessboard is detected in the two cameras before adding the calibration information. It prevents a wrong synchronization of the calibration information.
By the way, an action was also added in the extrinsic view to save the calibration images.

Fixes #292

### fwDicomIOFilter

*Limit memory usage of fwDicomIOFilterTest & fwDcmtkIOTest.*

This replaces the usage of deepCopy in two Dcmtk filters. We used to copy the whole DicomSeries and then remove the internal buffer. This is both inefficient and memory expensive. We propose here to use shallowCopy() instead, which does not copy the buffer. The container is still cleared out, but only the pointers, so the source buffer is not destroyed.

### Ogre

*Set the origin of the SNegato2D to a corner.*

The origin of the `::visuOgreAdaptor::SNegato2D` representation is moved on the lower left corner of the image. This change is necessary because the distance system (which was designed for the VTK backend) returns values from the lower left corner of the image. To get this working properly with Ogre, we need to change the origin that currently lies in the middle of the image. The method used to change the orientation of the negato was also modified accordingly.

### videoRealSense

*Fix grabber to properly update the cameraSeries.*

Prevent to add multiple times the same camera in the cameraSeries when the `startCamera` method is called by checking if the cameraSeries is calibrated.
Fix ExRealSense configuration to set the proper parameters for the service.

### unit-tests

*Fix random failures.*

- **fwServicesTest**: add missing wait. Also, one second might not be enough when the system is under heavy load, so the waiting time has been increased to 2500ms instead of 1000ms. Remember this is just the worst case and usually the function returns in a shorter time.
- **ioAtomsTest**: this divides the test image size by 2 and reduces the number of reconstructions from 15 to 5 in the test model series. This should help a bit to reduce the execution time, see results below.
- **igtlProtocolTest**: actually fix a lot of the mesh conversion code. It crashed randomly, but only because the UTest was very poor. If done properly, it should have shown how bad the code was, since apart from positions and cells copy, all the rest of the array copies were broken.
- **All tests**:
  - when destroying a worker, we may ask it to join twice the thread if someone else is calling `stop()`. On top of that, hen stopping a worker, we decide whether we need to join the thread by testing the state of the io_service. However this does not seem to be a reliable way, so we now test the thread state itself,
  - when launching two unit-tests in parallel, they may stop at the same time. Both processes will try to remove temporary folders simultaneously, so we must handle failures properly with exceptions,
  - use temporary folders for writing instead of folders in the build tree,
  - last, we realized it is a bad practice to rely on stopping a worker with the auto-destruction (i.e. using shared pointers), because this could lead the `std`::thread``to be destroyed from its own execution scope. So we deprecated the call of `stop()` in the `WorkerAsio` and now we advise people to call `stop()` from the callee thread (most often the main thread).

All of this solves a lot of random errors when lauching all unit-tests. We can also launch unit-tests in parallel now, making the CI jobs faster.

### SDK

*Remove relative paths in 3rd part libraries.*

All paths relative to the build host should now be removed from the include and library paths. The absolute paths are stripped away.
Some cleaning has also been done to include only the needed modules for VTK and PCL.

### qml

*VisuVTKAdaptor crashes when using QML.*

Registers imageSeries and tf objects in the SImageSeries constructor.

### Qml

*Fix SIOSelector when using Qml.*

Change the `AccessType` of the registered object from INPUT to INOUT when we are in writer_mode.

### Ogre

*Correct GLSL shaders compile errors on Intel chipsets.*

### SGrabberProxy

*Fix SGrabberProxy configuration selection.*

### SScan

*Convert point cloud positions to millimeter.*

RealSense camera return the point cloud in meters, but we need a point cloud in millimeters.
Thus we convert point positions to millimeter (multiply the values by 1000).
We also add a test if the camera is already started to avoid a crash.

## New features:

### calibrationActivity

*Improve widget layout.*

* Display all widgets other than the scenes in a panel on the left side.
* Fix the naming convention for service uids.

### cmake

*Add option to build a project with warnings as errors.*

* Added a cmake option `WARNINGS_AS_ERRORS` in sight to build a project in warning as error (`/Wx` on MSVC or `-Werror` on gcc/clang)
* Removed warning in sight project `fwlauncher`
* Enabled option `WARNINGS_AS_ERRORS` in `fwlauncher` project
* Changed warning level to `w4` on Windows (to see unreferenced local variable)

### cmake

*Improve package version number generation.*

* Added cmake helper script `get_git_rev.cmake` (from [sight-deps](https://git.ircad.fr/Sight/sight-deps/blob/dev/cmake/utils/get_git_rev.cmake)) to find git tags, branch name, version...
* Updated cmake script to generate a SDK filename using the latest tag (or git revision, see command [git describe --tags](https://git-scm.com/docs/git-describe))
* Updated cmake script to add Sight version in app packages

### cmake

*Add warning for links between bundles.*

Add a test in CMakeLists.txt to display a warning message if there are links between bundles

### fwRenderOgre

*Add a depth technique to the VR.*

Add a new technique to the volume rendering in order to display the depth of the volume.

* RayTracedVolumeDepth_FP: new fragment shader that displays the volume depth.
* OffScreenRenderWindowInteractorManager: fix the rendering by calling the specific target instead of all targets.

Remove a double connection in `::visuOgreAdaptor::SInteractorStyle`.

### CMake

*Allow utilities to be launched from Visual Studio.*

Generate the `vcxproj.user` for utilities.

### OgreDynamicImageTest

*Speed-up copyNegatoImage() and updateImage().*

- Improve copyNegatoImage using parallel omp

### videoRealSense

*Align pointcloud on RGB frame for AR.*

Align streams to the desired frame coordinate system (Depth, Infrared, Color).
This allow us to have each stream in the same coordinate system, no need to apply transforms in xml configurations.

New option is available trough ExRealSense: "Align frames to"; this allow user to choose target coordinate frame where all frames should be align to.

Add "visible" option in configuration of `::visuOgreAdaptor::SPointList`

### SHybridMarkerTracker

*Extract tag position and orientation in camera.*

* Modify `SHybridMarkerTracker` to extract tag position and orientation and take into account an `::arData::Camera`.
* Configuration file was replaced by parameters that could be set through `SParameters`.

### hybridMarkerTracker

*Add service to track cylindrical hybrid marker.*

- Add SHybridMarkerTracker service for tracking a cylindrical hybrid marker
- Add associated ExHybridMarkerTracker example

### visuOgreAdaptor

*Addition of a distance measurement editor.*

  -  Addition of a distance measurement adaptor in the Ogre 2D negato. You can create a new distance, remove a specific distance or hide/show the distance.

### visuOgreAdaptor

*Support landmarks visibility.*

Added landmark visibility support in `::visuOgreAdaptor::SLandmarks`

### conan

*Update all package to allow sharing of C flags.*

  -  The goal of this code is to share C flags across all our conan packages to ensure compatible code generation as some compiler settings can lead to strange bugs, hard to debug (especially floating point mode like `-mfpmath=sse`, please see the associated issue https://git.ircad.fr/Sight/sight/issues/188).

  -  We choose to write the flags and compiler settings inside a python file packaged as a conan package (https://git.ircad.fr/conan/conan-common/tree/stable/1.0.0). For now, you will have acces to:

  -  `get_[c,cxx]_flags()`, `get_[c,cxx]_flags_[release,debug,relwithdebinfo]()`, `get_cuda_version()`, `get_cuda_arch()` and some utility functions for conanfile.py like `fix_conan_path(conanfile, root, wildcard)` which allows to fix path in .cmake files.

### SFrameMatrixSynchronizer

*Handle time shift delay.*

Handle synchronization issues in our application by applying a time shift value.

### fwRenderOgre

*Update `MeshPickerInteractor` to send `PickingInfo` over signals.*

Send ``::fwDataTools::PickingInfo``with `MeshPickerInteractor`to be more generic.

### SLandmarks

*Configurable text size.*

Use an xml configuration to set the text size of `::visuOgreAdaptor::SLandmarks`.

### visuOgreAdaptor

*Addition of a dashed line SLine.*

Ability to draw a dashed line and choose the distance between the dots. Configuration is done in XML (SLine service)

### videoRealSense

*Implement the record and playback from realsense library.*

Adding record/playback functionalities in ::videoRealSense::SScan.

* Record function can records all streams synchronously in one file in rosbag format (.bag)
* Playback function can replay recording and emulate a realsense device (filters can be applied, and camera parameters are the same as the real device).

Also add color on Ogre's SPointList adaptor.

### visuOgreAdaptor

*Adaptor displaying text.*

Display text along the borders or in the center of an OGRE window.

### videoRealSense

*Add filters on depth frame.*

Allow to filter the depth frame with the three filters provided by the RealSense SDK: spacial, temporal and hole filling. The filters can be enabled and configured with a `SParameter`.

The three filters are:
* **Spatial Edge-Preserving filter**: it performs a series of 1D horizontal and vertical passes or iterations, to enhance the smoothness of the reconstructed data.
* **Temporal filter**: it is intended to improve the depth data persistency by manipulating per-pixel values based on previous frames. The filter performs a single pass on the data, adjusting the depth values while also updating the tracking history. In cases where the pixel data is missing or invalid the filter uses a user-defined persistency mode to decide whether the missing value should be rectified with stored data. Note that due to its reliance on historic data the filter may introduce visible blurring/smearing artifacts, and therefore is best-suited for static scenes.
* **Holes Filling filter**: the filter implements several methods to rectify missing data in the resulting image. The filter obtains the four immediate pixel "neighbors" (up, down ,left, right), and selects one of them according to a user-defined rule.

Update `ExRealSense` to add widgets to change the filters parameters.

Add a service configuration in ARCalibration to play the RealSense infrared frame without the emitter.

Fix `SPointCloudFromDepthMap` to emit less signals. It emitted 'modified' signal on the mesh on each update which was causing the entire refresh of the VTK mesh (very slow). But only the vertex and the point colors are modified, so now, only 'vertexModified' and 'pointColorModied' signal are emitted.

### Conan

*Update many conan packages.*

Update conan package to more up-to-date versions and refactor sight a bit:

* boost --> 1.69
* eigen --> 3.3.7
* Qt --> 5.12.2
* pcl -->1.9.1
* vtk --> 8.2.0
* itk --> 4.13.2
* opencv --> 3.4.5
* ogre --> 1.11.5
* glm --> 0.9.9.5
* gdcm --> 2.8.9
* dcmtk --> 3.6.4
* odil --> 0.10.0
* cryptopp --> 8.1
* bullet --> 2.88

Details:

* **boost** related 
### fwIO

*Implement a fail/sucess notification on all readers and writers.*

Adds a new member status boolean to `IReader` and `IWriter`and all of their inherited implementations.

The following implementation of those interfaces have been modified :

Readers :

* ioAtoms/SReader
* ioCalibration/SOpenCVReader
* ioData/SAttachmentSeriesReader
* ioData/STrianMeshReader
* ioData/TransformationMatrix3DReaderService
* ioDcmtk/SSeriesDBReader
* ioGdcm/SDicomSeriesDBReader
* ioGdcm/SSeriesDBReader
* ioITK/InrImageReaderService
* ioITK/SInrSeriesDBReader
* ioITK/SImageReader
* ioVTK/SModelSeriesReader
* ioVTK/SSeriesDBReader
* ioVTKGdcm/SSeriesDBLazyReader
* ioVTKGdcm/SSeriesDBReader

Writers :

* ioAtoms/SWriter
* ioCalibration/SCalibrationImagesWriter
* ioCalibration/SOpenCVWriter
* ioData/MeshWriterService
* ioData/TransformationMatrix3DWriterService (great name btw).
* ioGdcm/SDicomSeriesWriter
* ioGdcm/SSeriesDBWriter
* ioGdcm/SSurfaceSegmentationWriter
* ioITK/InrImageWriterService
* ioITK/JpgImageWriterService
* ioITK/SJpgImageSeriesWriter
* ioITK/SImageSeriesWriter
* ioQt/SPdfWriter
* ioVTK/SImageSeriesWriter
* ioVTK/SImageWriter
* ioVTK/SMeshWriter
* ioVTK/SModelSeriesObjWriter
* ioVTK/SModelSeriesWriter
* ioVTKGdcm/SImageSeriewWriter

The following implementation of those interfaces have **not** been modified : Note: by default we consider that a non modified reader always return success...

Readers:

* ioTimeline/SMatricesRead : this service is to complex for a simple muggle like me.
* ioZMQ/SAtomNetworkReader : one of those that do a bit more than reading... :rolling_eyes:
* ioZMQ/SImageNetworkReader : same here

Writers:

* ioTimeline/SMatrixWriter : see above
* ioZMQ/SAtomNetworkWriter : idem
* ioZMQ/SImageNetworkWriter : idem
* videoOpenCV/SFrameWriter : not the actual behaviour we expect.

### videoVLC

*Implement RTP video streaming with libvlc.*

- Adds a new service ̀SFrameStreamer in the videoVLC bundle. Which allows to stream frames pushed into a FrameTL.
- Adds a new sample ExVLCVideoStreaming that grabs and streams a given video (either passed through a file or directly a stream).
- Adds and increments videoVLC conan package version dependency.

### conan

*Stores all used versions of conan packages in a single file.*

To simplify the process, all versions packages of conan are now merged into a simple conan-deps.cmake file

### GUI

*Add tooltips on views.*

A tooltip can now be set on any view of a cardinal or a line layout manager, optionally of course :

```xml
            <service uid="multiView" type="::gui::view::SDefaultView">
                <gui>
                    <layout type="::fwGui::LineLayoutManager">
                        <orientation value="horizontal" />
                        <view proportion="0" minWidth="30" toolTip="Amazing view" />
                        <view proportion="0" minWidth="50" />
                        <view proportion="1" toolTip="Please click me"/>
                    </layout>
                </gui>
                <registry>
                    <view sid="subview" start="yes" />
                    <view sid="button" start="yes" />
                    <view sid="button2" start="yes" />
                </registry>
            </service>
```

If the editor inside the view already has a tooltip, it will be not be overridden by this configuration, thus we do not break the existing tooltips.

### gitlab-ci

*Check the compilation with SPYLOG_LEVEL=trace.*

Update gitlab-ci to compile debug configuration with SPYLOG_LEVEL=trace to check wrong log message.
Fix a wrong debug message.

### SGrabberProxy

*Forward IGrabber's signals.*

### FrameLayoutManager

*Add visibility parameter.*

### opencv

*Use our own ffmpeg and rework opencv dependencies.*

### calibration

*Write calibration input images to a folder.*

Adds a feature to write calibration input images to a folder.
This can be useful for debugging or to compare our results with those of third-party programs.

Calibration info image writer.

### CI

*Add a new stage to build CI on linux to build the sdk.*

Besides this this also embeds some fixes for the sdk packaging on windows.

### test

*Use always slow tests skipping.*

Resolve "ease CI for slow test"

### fwTest

*Add a template version of randomizeArray.*

Add a template version of randomizeArray in fwTest generators.
It is useful for floating arrays because with the existing method some 'nan' values could appear, and the comparison of 'nan' return always false.

### TutoSimpleAR

*Promote ExSimpleARCV into a real tutorial.*

ExSimpleARCV is promoted to a real tutorial named TutoSimpleAR. The configuration has been cleaned a bit, comments have been added everywhere and the sample data are now downloaded at configuration time to help the beginners.
This sample is also a demonstration of the new design to synchronize AR rendering efficiently.
As most of the tutorials, it is documented in the official documentation.

## Refactor:

### deprecated

*Remove deprecated code related to sight 19.0.*

* Remove deprecated methods in `fwDcmtkIO`
* Remove outdated and unused bundle `uiNetwork`

### SActivitySequencer

*Emit a signal when the activity requires additional data.*

The service emits a signal 'dataRequired' if the activity can not be launch because some data are missing.
It allows to connect it to a SActivityWizard to select new data.

### conan

*Remove sight-deps support, conan is used by default.*

Conan is now the official way to retrieve 3rd party libraries in Sight.
Since it would be a pain to keep maintaining both sight-deps and Conan together,
we agreed to skip the normal deprecation phase.

* Remove option `USE_CONAN` in sight and make it the default.
* Clean all parts of the build system that refer to `EXTERNAL_LIBRARIES`.
* Remove support of Eclipse project

### fwPreferences

*Add getValue helper.*

The method `getPreferenceKey(...)` returns associated value saved in the preferences (if delimited with the character `%`) or simply returns the variable.
This helper is defined (copied/pasted) 12 times in Sight.
To avoid this, we have centralized all these versions in `fwPreferences` with the new method `getValue(const std::string& var, const char delimiter = '%')`.

### deprecated

*Remove deprecated code related to sight 19.0.*

### trackingCalibration

*Remove hand-eye reprojection service.*

SChessboardReprojection was unused in sight.

Remove the hand-eye reprojection service

### videoCalibration

*Rewrite chessboard detection.*

Modifies the SChessboardDetection service to take images as inputs instead of timelines.
Makes synchronization easier.

### RayTracedVolume_FP.glsl

*Remove IDVR referencies.*

## Performances:

### fwVtkIO

*Improve mesh conversion.*

The methods updating the mesh points, normals and texture to VTK were very slow, checking the allocated memory on each value. It is replaced by using the `SetArray()` methods with a copy of the data mesh array. This increases the speed from ~23ms to ~0.7ms for the point color update.


# sight 18.1.0

## New features:

### MarkedSphereHandleRepresentation

*Add ComputeInteractionState method.*

### videoRealSense

*Update realsense grabber:.*
- grab also pointcloud
- live loading of presets (in /rc/presets)
- enable/disable IR emitter
- switch between color/infrared frame
- live modification of min/max range
- speed-up the grabbing function
- brand-new ExRealSense using Ogre backend

Resolve "Output pointcloud from realsense grabber"

### ci

*Add ci-jobs to build sight on windows and macos.*

- Add some jobs to build sight on Linux, Macos and Windows
- Launch unit-test
- Use ccache to reduce build time

Resolve "Add windows and macos as gitlab CI target"

### CMake

*Add source packaging with CPack.*

It is now possible to create a source package for a specific application.
To do so, you have to:

- Choose an app you'd like to package. (e.g. VRRender, OgreViewer,...) This needs to be an **installable** app make sure it calls `generic_install` in its `CMakeLists.txt`
- Set it (and only it) as the `PROJECTS_TO_BUILD` in your cmake config.
- Run cmake `cmake .`
- Build the source archive, if building with ninja you can run `ninja package_source` else run `cpack --config CPackSourceConfig.cmake`
- Retrieve the source archive (e.g. VRRender-0.9-Source.tar.gz if you chose VRRender).

## Some results

A source package example : [PoCRegistration-0.1-Source.tar.gz](/uploads/a4fdd05b1681b7f01c1c553903f0f072/PoCRegistration-0.1-Source.tar.gz)

## Additional tests to run

This should better be tested on all platforms.

- [x] Windows
- [ ] ~~macOS~~ :arrow\_right: won't work see #248
- [x] GNU/Linux

### SDecomposeMatrix

*New service to decompose a matrix and associated tutorial.*

Add a new service to decompose a matrix into a rotation matrix, a translation matrix and a scale matrix.
Add a new sample using this service.

### TabLayoutManagerBase

*Add a border option.*
- add also Parameters_Gray.svg icon.

Resolve "Improve GUI esthetic"

### SParameters

*Add new index based enumeration slot.*

Add a slot which changes enumeration state based on the index and not on the label.

### video

*Add slot that plays or pauses according to current state.*

- Add new slot to IGrabber that plays or pauses.
- Add two state booleans on IGrabber.
- Add declaration to start, stop and pause on IGrabber.
- Add parent call to all derived grabbers for start, stop, pause.

### conan

*Find conan packages associated with installed cuda version.*

- Use conan option 'cuda' to find package associated with cuda version installed on the machine
- Available cuda versions: "9.2", "10.0", "None"

### SFlip

*Add service to flip images.*

- Add a service to flip images in the three main axes (can handle up to 3D images)
- Implement the SFlip service in Tuto06Filter

### vscode

*Add vs code generator.*

- add VS Code support to build and debug Sight
- to use VSCode with sight: run CMake configure with option GENERATE_VSCODE_WS, and open the workspace file "sight.code-workspace" (in build dir)

### conan

*Add linuxmint19 support.*

- Support new settings 'os.distro' in cmake-conan script into sight for linux.
- Available values in your packages for settings 'os.distro' are:
  - linuxmint18
  - linuxmint19

### idvr

*Draw the IDVR's depth lines from the outside of the countersink.*

### visuOgreAdaptor::SPointList

*Addition of a labeled point list.*

If the option "displayLabel" is set to true on the XML configuration, it will add a label on a point when the point is created.
You can also choose the character height and the color of the label on the XML configuration.

### PoCMergePointcloud

*Add a new PoC to merge pointclouds from 2 RGBD Cameras (Orbbec astras).*

Move opDepthMap & depthMapOp from internal repository to opensource.

Resolve "Sample to merge two RGBD Cameras"

### Calibration

*Export extrinsic matrix in .trf format.*

Resolve "Export extrinsic matrix in .trf in calibration activity"

### OGRE

*Configure stereo cameras using camera series.*

- Computes each projection matrix using intrinsic and extrinsic calibrations

### Ogre

*Improve stereo rendering management.*

- Adds an action to enable/disable stereo in an ogre layer.
- Refactors the stereo mode to be handled by the core compositor.
- Fixes some adaptors that crashed when restarting.

### SImageWriter

*Handles timestamps and bitmap images".*

### Player

*Add getter of video duration.*

Adds a getter for QMediaPlayer::duration() this is useful for knowing how much time is left on your video and especially this ticket for a video timeline editor which basically uses the `::fwVideoQt::Player`to get specific frames at specific positions.

## Bug fixes:

### template.sh

*Add quotes in linux sh template (build & install).*

Resolve "Wrong bundledir path in application scripts automatically generated by CMake"

### OgreViewer

*Correct user interface bugs.*

The compositor uniforms are now properly shown in the compositor selector.
The application no longer crashes at exit when all lights are removed.

### SMaterial

*Use a lookup table to correctly display 16 bits texture.*

### videoOpenCV

*Duration calculation & loop mode.*
* Correct the duration calculation.
* Use total number of frame and current frame index to loop the video.

Resolve "OpenCV grabber loop mode & duration"

### guiQt

*Copy the styles plugin on install.*

Installed and packaged apps looked bad because we forgot to ship the qt styles plugin.

### fwRuntime

*Remove windows.h in Library.hpp.*

- avoid exposing of windows.h in fwRuntime`::dl::Library`header (via dl::Win32.hpp)
- use abstract class fwRuntime`::dl::Native`(as a pointer) in the hpp and instantiate the specific implementation in the cpp
- update vlc conan package used in videoVLC to fix problems related to windows.h

### Flipper

*Move the implementation to the correct path.*

Resolve "Move Flipper.cpp from imageFlipperOp to imageFilterOp"

### videoOrbbec

*Allow to use the grabber in RGB mode.*

Fixes a crash in videoOrbbec`::SScan`if no depth timeline was given. Now, we can use this grabber to get only the RGB frames.

### ci

*Fix job to deploy doxygen on the sight gitlab page.*

- Doxygen pages can no longer be deployed on the CI due to an error on the gitlab-ci script.
- There is an error because the git configuration has been defined globally for all jobs, and the deployment jobs use a docker image that doesn't contain 'git'.
- To fix this, we'll use the same docker image for all linux jobs (DOCKER_ENVDEV_MINT19)

### guiQtTest

*Add missing profile.xml and gui requirement.*

### fwDicomIOFilter

*Fix unit tests.*

### SStereoToggler

*Remove fwServicesRegisterMacro macro.*

Fix compilation without PCH by removing the fwServicesRegisterMacro

Resolve "Missing header in SStereoToggler.cpp"

### utests

*Various fixes for fwJobsTest, fwGdcmIOTest, fwRenderOgreTest, fwThreadTest.*

This fixes the following unit tests:

- fwJobsTest: The problem was visible on macOS, but other platforms maybe impacted. Testing if the future is valid before waiting did the trick. Also a wrong "+" with a char* and an integer which give also a crash in release, has been replaced with a proper string concatenation

- fwGdcmIOTest: The hashing function boost::hash_combine() doesn't always use the same implementation across platforms (!!!). We now use a regular sha1 hashing, a bit slower, but safer

- fwRenderOgreTest: Deleting the dummy RenderWindow with destroy() leads to a double delete crash when deleting ogre root node. Using the appropriate Ogre::RenderSystem::destroyRenderWindow() seems to fix the crash

- fwThreadTest: Use a workaround to mitigate the callback cost in time computation. This is a workaround because either the test itself is false (and maybe undoable, as unpredictable), either we must change the implementation of TimerAsio to take into account the time taken by the callback. We may look again for https://git.ircad.fr/Sight/sight/tree/fix/TimerAsio, but it is clear this change will have side effect since some code may actually want to wait for the callback to perform before reseting the timer. I guess that playing a video with openCV did fail with the above merge request

### cmake

*Use the correct macOS flags to build pch.*

Pass the correct macOS flags when compiling pch

### gitignore

*Parse also versioned qtcreator cmake preferences files.*

Resolve "Update gitignore to include qtcreator's versioned files"

### conan

*Quiet conan output.*

Update cmake script with conan OUTPUT_QUIET option to have a more quiet conan's output

### GridProxyGeometry

*Fix variables declaration.*

### SGrabberProxy

*Correct parsing of grabber list and ensure that the list is filled even if no config is set.*

Resolve "Empty SGrabberProxy list"

### ci

*Update ccache path.*

- now ccache use a nfs folder mounted in docker image
- previous gitlab-ci cache is not compatible with this system

### SFrameWriter

*Use the correct timestamp in image filename.*

Use the timeline timestamp instead of the current timestamp when saving frames using videoOpenCV::SFrameWriter

## Refactor:

### Ogre

*Remove IDVR from the repository.*

This MR removes all IDVR references from OgreViewer. This makes the application simpler,
because over time it accumulated too many features.

In the same time, we added volume rendering and negatoMPR adaptors to the VTK tab,
in order to show the comparison between the two back-ends.

OgreViewer has been updated to 0.3.

### ui

*Remove deprecated services (ShowLandmark, SaveLandmark, LoadLandmark and DynamicView).*

Resolve "Remove deprecated services"

### Bookmarks

*Deprecate ::fwTools::Bookmarks.*

Add FW_DEPRECATED macro in fwTools`::Bookmarks`and the associated service ctrlSelection::BookmarksSrv.
The bookmarks are no longer used and the service still use a deprecated API.

### HiResClock

*Undeprecate getTime* functions.*

Resolve "Un-deprecate HiResClock functions"

### videoRealSense

*Use the version 2 of the realsense api.*
This is a basic implementation of a Grabber for D400 cameras:

* parameters such as resolution, fps, ... are set in code
* the camera chosen with the videoQt selector is not taken into account by the grabber (Qt doesn't recognize correctly realsense device)
* if multiple realsense cameras are plugged-in the grabber will pop a selector dialog
* depth pointcloud is not outputted for now

### filterVRRender

*Rename filterVRRender to filterUnknownSeries.*

`FilterUnknownActivities` filter from `filterUnknownSeries` bundle allows
to remove the unknown activities when reading a medical file (.json,
.jsonz, .apz, ...) with `::ioAtoms::SReader`.

The old filterVRRender bundle is kept for backward compatibility, but will
be removed in version 19.0.

### ioGdcm

*Remove plugin.xml in ioGdcm.*

Remove the plugin.xml file in ioGdcm bundle because it is useless (cmake generates it automatically).
Also change the documentation of services.

### uiVisuOgre

*Remove obsolete getObjSrvConnections().*

### fwTools

*Move shared library path search code and make it generic.*

This introduces a new function in `fwTools`::Os``:

```cpp
/**
 * @brief   Return the path to a shared library name
 * The library should have already been loaded before.
 * @param _libName The name of the shared library, without any 'lib' prefix, 'd' suffix or extension,
 * i.e. 'jpeg' or 'boost_filesystem'. The function will try to use the appropriate combination according to
 * the platform and the build type.
 * @return path to the library on the filesystem
 * @throw `::fwTools::Exception`if the library could not be found (not loaded for instance)
 */
FWTOOLS_API `::boost::filesystem::path`getSharedLibraryPath(const std::string& _libName);
```

This function has been used to search for Qt plugins in `WorkerQt.cpp`,
where this was originally needed. It can be used in some of our utilities as well. The code is generic and has been unit-tested properly.

### video

*Better support of selected device in video grabbers.*

* Strongly identify camera when using Qt Selector, and add better support of multiple same model cameras

  * Refactor CameraDeviceDlg to keep usb order in comboBox
  * Uniquely identify cameras using a prefix pushed in description.

* Grab Webcam with OpenCV is now multiplatform.

* Enhance the video support when using OpenNI grabbers.

  * Add support of multiple connected astra (videoOrbbec)
  * Use Opencv instead of Qt to grab RGB frame
  * Grab RGB & Depth frame in same thread (synchronized on depth)
  * Remove Qt in videoOrbbec

* Add Astra utility program to replace over-complicated SScanIR

  * astraViewer: to view Color/IR and Depth stream
  * astraRecord3Streams: to display Color + IR + Depth and take a snapshot (! FPS will be very very slow ! ).

Resolve "videoOpenni and videoOrbbec SScan don't take into account the selected device"


# sight 18.0.0

## Bug fixes:

### STransform

*Check parent node existence before creating it.*

### unit-tests

*Look for profile.xml in the source directory.*

Instead of looking for a profile.xml in the build directory for the unit-tests, now we look into the source directory. Otherwise, this does not work when CMake is only ran once.

Doing this, some missing bundle requirements in unit-tests were fixed.

### CMake

*Check if profile exists to enable bundles load on unit-tests.*

### *

*Add GL_SILENCE_DEPRECATION definition globally.*

Before it was only defined in fwRenderOgre.

Resolve "OpenGL is deprecated on MacOS 10.14"

### TutoOgreGenericScene

*Update deprecated service key.*

This made this application crash.

### install

*Remove useless install target.*

setpath.bat and <app_name>.bat are only usable in build dir

### unit-tests

*Ensure test data are really free and anonymized.*

This anonymize some data used in our unit-tests.

### SMesh

*Check if node exists before attaching it to root.*

### DCMTK

*Change DCMTK files path on Windows.*

### *

*Resolve errors reported by FOSSA.*

- Update our license headers to mention Sight and not FW4SPL
- Change the license headers to follow [GNU recommendations](https://www.gnu.org/copyleft/lesser.html)
- Add IHU Strasbourg copyright
- Remove LGPL license headers from minizip files
- Remove DCMTK files and copy them at build instead
- Add copyrights for MIT licensed files.

### fwVideoQt

*Properly close the camera device and deallocate the related resources.*

Resolve "ARCalibration crash when stream stops"

### macos

*Fix OgreViewer for macOS in debug mode.*

### calibrationActivity

*Correct intrinsic editor inputs.*

Calibration activity crashes when starting the intrinsic editor because of an input that should be an inout.

Merge remote-tracking branch 'origin/fix-calibration-activity-crash-on-start' into dev

### SReprojectionError

*Fix indexing of marker points.*

### fwRuntime

*Resolve NOMINMAX redifinition.*

- NOMINMAX was defined twice (in 'fw-boost.cmake' and in 'fwRuntime/dl/Win32.hpp')
- a warning C4005 was displayed each time this file is used

### conan

*Update conan packages.*

Conan has new packages available, it is necessary to update the versions used by Sight for macos and QML

### ImportanceCompositing_FP

*Allow Nsight compiling.*

### SActivityWizard

*Fix crash when loading or importing a data.*

Fix load and import in SActivityWizard. SIOSelector service must be configured before to associate the output object Id.

### CMake

*Use the correct variable for deps without Conan.*

We just used the wrong variable...

### visuOgre

*Add include file.*

### SCamera

*Replace assert by warning when creating cameras.*

Allow to use ``::videoQt::editor::SCamera``when we don't know if the input CameraSeries will be initialized or not: the cameras will be generated if the cameraSeries is empty.

### ogre

*Fix naming conflicts and double deletes.*

### sight

*Set external lib dir in scripts to launch Sight apps.*

Scripts used to launch Sight applications are not correct because the variable FW_EXTERNAL_LIBRARIES_DIR is empty.

### fwRenderOgre

*Silent deprecated OpenGL warnings on macos.*

Resolve "[macos 10.14] OpenGL is deprecated"

### tests

*Fix fwServicesTest and fwActivitiesTest.*

Fix the broken tests of `fwServicesTest` and `fwActivitiesTest`:
- fix the resources path
- update the wait macros to test if a service exists. Replace `::fwTools::fwID::getObject(uid)` by `::fwTools::fwID::exist(uid)` because when getObject() is called, an assert is raised: the service pointer expires before it is removed from the ID map.

### CMake

*Remove usage of QT_QPA_FONTDIR on Linux.*

Usage of **QT_QPA_FONTDIR** is deleted in both build & install scripts
since now we build Qt with fontconfig support.

### fwRenderOgre

*Move declaration of static const outside the class.*

### Ogre

*R2VB objects can't be picked.*

This allows quad-based meshes to be picked. Since the quads are triangulated with a geometry shader, it was not that easy to perform. The chosen solution is not very elegant, I added intersection tests on the quads themselves. Testing intersections on the triangulation is harder to perform and on top of that, at the beginning we did not intend to read it from the CPU.

### IDVR

*Mesh/countersink interference.*

Also brings a few new buttons to the OgreViewer toolbar and fixes the clipping box update.

Only works with the Default or the DepthPeeling transparency technique.

[Ogre] Fix idvr with mixed rendering

### SPointListRegistration

*Test if registered pointList has labels.*

Fix SPointListRegistration service when having two pointLists which one has labels and the other one has no labels.

Resolve "Fix SPointListRegistration"

### fwDataToolsTest

*Glm initialization.*

### CMakeLists

*Add a define for aligned storage compatibility with visual 2017.*

### macos

*Remove installation script for executable.*

Remove installation script for executable for macOS.
This script did not work and broke the cmake configuration. It should be rewrite completely.

See #131

### export

*Fix export macro in arData and calibrationActivity.*

### cmake

*Allow use of relative path for EXTERNAL_LIBRARIES.*

Resolve "Cannot use relative path for EXTERNAL_LIBRARIES"
### ogre

*Fix relative paths on linux.*

material paths are now absolute using predefined variables

### ogre

*Add missing makeCurrent() and fix glsl errors.*

### CMakeLists.txt

*Fix link errors with vtk libraries.*

The PCL discovery was done in the main CMakeLists.txt, thus defining VTK compile definitions for all targets. This led to strange link errors, notably in unit-tests.

## New features:

### conan

*Implement a better Conan support, especially on macOS.*

- Remove libxml2 and zlib dependencies on macOS
- Use DYLD_FALLBACK_LIBRARY_PATH instead of LD_LIBRARY_PATH because it is unsupported in macOS and DYLD_LIBRARY_PATH prevents using ANY system libs.
- Remove unneeded activities bundle dependency to gui
- Fix application script template generation to use : as path separator instead of ;

- Cleanup template script
- Add VLC conan package for Windows and macOS, use system for Linux
- Add scripts to launch unit tests and utilities with conan packages

- bonus: fix ioAtoms and ioITK unit tests (updated object access type)

### doxygen

*Generate sight doxygen with Gitlab-CI.*

- A new Gitlab-CI job is added to generate and publish Sight Doxygen
- Doxygen is built only if sheldon and debug/release build pass
- Doxygen is only deployed on the dev branch
- Sight doxygen is available on https://sight.pages.ircad.fr/sight

### tutoQml

*Use qml and c++ for tuto 08 and 09.*

New version of the Tutorial 08 and 09 using qml and c++ (without XML configuration).

- fwVTKQml:new library to allow to display a VTK scene in qml
- visuVTKQml:new bundle to initialize qml FrameBuffer
- uiMedDataQml, uiImageQml, uiReconstructionQml: new bundles containing services inherited from IQmlService and the associated qml files.

### tutoQml

*Add basic samples using qml interfaces.*

Add new samples to use Qml:
- `Tuto01BasicQml`: same as Tuto01Basic but using qml instead of fwGui frame and xml
- `TutoGuiQml`: same as TutoGui but using qml instead of fwGui frame and actions
- `TutoEditorQml`: sample to explain how to use a IQmlEditor

Add a new library `fwQml` that contains helpers to use qml:
- IQmlEditor: base class for f4s service associated to qml UI
- QmlEngine: to launch qml ui

### tutoCpp

*Convert tuto 08 and 09 to use C++.*

The new Samples `Tuto08GenericSceneCpp` and `Tuto09MesherWithGenericSceneCpp` are equivalent to the existing samples (same name without 'Cpp') but they doesn't use a XML configuration.

All the services are managed in C++ in the application `Plugin` class. The services' configurations are written with `::boost::property::tree`.

To achieve this, a new helper : ``::fwServices::AppManager``was added. It simplifies the management of objects/services/connections in an application.

### CMake

*Use Conan to build Sight (experimental).*

Add conan support in sight:
- A temporary conan registry is used for the moment: [Artifactory](http://5.39.78.163:8081/artifactory)
- All conan package are available on [gitlab Conan group](https://gitlab.lan.local/conan/)
- A new advanced CMake option is available: `USE_CONAN`

### trackingHandEyeActivity

*Load and evaluate calibration matrices.*

Adds the possibility to load the hand-eye X matrix for evaluation.

Once loaded the Z matrix is computed using the current tracked position and camera pose.

The camera can than be moved around and the app will display the reprojection and reprojection error.

Evaluate external calibrations in the hand-eye activity.

### SLabeledPointList

*Implement updateVisibility slot.*
- to update visibility of SLabeledPointList, we have to implement updateVisibility slot into SPoint, SPointLabel, SPointList

Resolve "Implement updateVisibility slot in SLabeledPointList"

### PointList

*Add pointlist management methods.*

* Add a slot to clear the pointlist in SMarkerToPoint
* Add a slot to manage the visibility of the pointlist in the adaptor SPointList3D

### SRenderStats

*Display rendering stats such as FPS in the OGRE overlay.*

- Adds a new adaptor to display FPS and triangle count in ogre windows.
- Fixes a crash when setting the volume visibility when no image has been loaded.
- Removes the option to print stats in a console.

### sslotcaller

*Add wait option for synchronized slot calling.*

Add a wait option to SSlotCaller that is used to call the slots synchronously. This allows for dependent slots to be called in the right order.

### 21-integration-of-sopticalflow-from-internship-repository

*Into dev.*

Add a new service that performs optical flow on a video to detect if camera is moving or not.
Use the service in a new example called ExDetectCamMotion.

SOpticalFlow is designed to only detect if camera is moving not if something happend on video.
It can send 2 signals: cameraMoved if motion and cameraRemained if camera is stable.

This can be usefull to trigger event when motion or remaining phases are detected, ex: only add calibration image if camera is not moving.

### 124-sort-example-by-topics-and-remove-numbering

*Into dev.*

Remove numbering of Samples, and add topics folders.

Resolve "Sort example by topics and remove numbering"

### CMake

*Introduce SDK mode.*

After many years, we propose a real build of Sight as a SDK. This allows to:
- create a binary package containing Sight binaries, libraries, includes and resources,
- include/link a Sight library in a non-Sight application/library,
- create a new Sight library, bundle or application outside Sight source and build tree

### .gitignore

*Ignore vs2017 cmake project files.*

## Refactor:

### *

*Remove "fw4spl" references.*

Resolve "Remove all references to "fw4spl""

### ITransformable

*Node managements is now handle by ITransformable.*

* Factorise some functions to have a common method into ITransformable.
* Set the parent transformation only in STransform (else, two adaptors with the same transform can have two different parents).
* Fix the stopping method of SAxis.

### ioIGTL

*Modify INetwork and STDataListener to manage timestamps.*

- Add receiveObject function in INetwork to return the message timestamp
- Add optional timestamp parameter in manageTimeline from STDataListener

### deprecated

*Remove 'FW_DEPRECATED' related to 18.0.*

Remove the deprecated code associated to the macros `FW_DEPRECATED_xxx(..., "18.0)`.

### MedicalImage

*Split to MedicalImage and TransferFunction.*
Splits `::fwDataTools::helper::MedicalImageAdaptor`to `::fwDataTools::helper::MedicalImage`and ::fwDataTools::helper::TransferFunction

Most of the adaptors used the half of the `::fwDataTools::helper::MedicalImageAdaptor`: one part to manage an fwData`::Image`and the other one to manage fwData::TransferFunction.

Resolve "Split `MedicalImageAdaptor` into `MedicalImage` and `TransferFunction`"

### getObjSrvConnections

*Replace getObjSrvConnections() by getAutoConnections().*

Resolve getObjSrvConnections() errors by implementing getAutoConnection() method.

### fwCore::HiResClock

*Use std::chrono inside and deprecate in favor of std::chrono.*

### ogre

*Refactor volume ray entries compositors.*

- Removes the volume ray entry compositor scripts and generates them instead.
It will be a lot easier to add stereo IDVR now.

- Moves all programs/materials/compositors used in fwRenderOgre to it.

* **:warning: IMPORTANT** Delete share/fwRendreOgre and share/material in your build directory

### cmake

*Cmake cleaning.*

cleans sight CMake scripts:
- removed unused platform
- removed racy backward compatibility

### *

*Reorganise some folders.*

After the big merge with fw4spl-ar, fw4spl-ogre and fw4spl-ext, it was necessary to clean up a bit the folders hierarchy, notably to group related items together. No project is renamed, the projects were only moved or their parent directory was renamed.

See sight!2

## Documentation:

### README.md

*Remove all mentions of 'fw4spl'.*


# fw4spl 17.2.0

## New features:

### calDataGenerator

*Add an utility program to generate stereo pair of chessboard/charucoboard images.*

This can be useful to test calibration algorithms.

### handEyeActivity

*Handling a step value in SMatricesReader and SFrameGrabber.*

Add a step value in readNext()/readPrevious() slots in SMatricesReader and SFrameGrabber when configured on oneShot mode.

This step value can be changed calling a setStep slot, connected with an int SParameter withstep` key.

We also needed to add this setStep slot in the SGrabberProxy and IGrabber in order to call it properly when using a SGrabberProxy instead of a SFrameGrabber directly.

### Ogre

*Update ogre to 1.11.*

This brings a bunch of fixes following API changes. Among them :

* default light direction is set to the camera's view direction, was implicitly the case in ogre 1.10 but changed in 1.11
* `Codec_FreeImage` plugin loaded to support common image file formats
* plugin config parsing was modified to be able to load multiple plugins
* `::Ogre::Affine3` replaces `::Ogre::Matrix4` when we need to decompose a matrix
* colour masks are enabled when computing volume ray entry points

## Refactor:

### ut

*Replace deprecated methods to register a service.*

Replace the `::OSR::registerService(obj, srv)` by `srv->registerInOut(obj, key)`
in the unit tests.

### cmake

*Remove racy backward compatibility.*

### registerService

*Replace deprecated methods to register a service.*

Replace the `::OSR::registerService(obj, srv)` by `srv->registerInOut(obj, key)`
and use `::fwServices::add(srv)` helper instead of calling directly `ServiceFactory

### SVolumeRender

*Store clipping matrices the same way VTK does.*

Now clipping box transforms are stored in world space instead of texture space. Clipping transforms can be passed from/to VTK that way.

Removes the **broken** slice based volume renderer.

### plugins

*Remove the freeimage plugin.*

### textures

*Convert all png and tga textures to dds.*

DDS is supported natively by ogre without the freeimage plugin.

The freeimage BinPkg is awful to maintain and should be considered as deprecated from now on.


## Bug fixes:

### glm

*Add missing GLM_ENABLE_EXPERIMENTAL define.*

Unused glm extensions have been removed

### fwRuntime

*Fix memory leaks.*

Fix leaks in fwRuntime

### SMaterial

*Texture rendering on other formats than 8 bits.*

### IHasServices

*Add wait() when stopping services.*

- add wait() when stopping services in unregisterService and unregisterServices methods.

### SGrabberProxy

*Include/exclude mode wasn't working as expected.*

- Improve include/exclude filtering. We can include/exclude a specific service or a specific configuration of a service or both.
- Grabbers are now always displayed in same order in selector dialog.
- Frame by Frame mode from `::videoOpenCV::SFrameGrabber`has been excluded from calibration

### registrationActivity

*Fix all errors in the registration app.*

- fix all log errors
- remove useless autoConnects


# fw4spl 17.1.0

## Refactor:

### fwServicesTest

*Clean unit test and deprecate unused methods.*

Add `FW_DEPRECATED` macro for:
- swapService(obj, srv)
- registerService(obj, service)
- getServices(obj)
- getServices(obj, type)
- fwServices::add(obj, srvType, srvImpl)
- fwServices::get(obj)

Replace the deprecated methods in the tests by the new ones.
Replace configuration writen in C++ by XML file for the tests of AppConfigTest.

Keep a few tests on the deprecated methods until the methods are officially removed.

### VRRender

*Remove the deprecated logs.*

Clean the configurations to remove the deprecated logs:
- remove the useless objects and services
- use the right key
- remove useless autoConnect

Update appConfig.xsd to set 'uid' attribute as required for services.

Add missing 'getAutoConnections()' in some services from visuVTKAdaptor.

### tutorials

*Remove the deprecated logs.*

Remove the deprecated log:
- clean configurations
  - remove useless autoConnect
  - use the right keys in services
  - remove useless services and object
- add getAutoConnections() methods in uiReconstructionQt to replace the default deprecated getObjSrvConnections() from IService
- use the new API to register the reader/writer in ::uiTF::TransfertFunctionEditor
- remove auto-connection on 'tf' in the vtk adaptors when registering a sub-service

### ObjectService

*Support optional output in services.*

- if the output is not defined in the XML configuration, the object is not emitted to the configuration.
- add a method in IService to check if the object is defined: hasObjectId()
- the method getObjectId() throw an exception as described in the doxygen

### SWriter

*Set 'data' as input instead of inout.*

Set 'data' as input in '::ioAtoms::SWriter'
- update RecursiveLock visitor to use const object
- add constCast in SWriter before the conversion to atoms

### deprecated

*Remove deprecated getObject() in services.*

Replace 'getObject()' by 'getInput()' or 'getInout()' and add a deprecated
log if the input key is not correct.

### getObject

*Remove deprecated getObject().*

Replace deprecated `getObject()` by `getInout()` in `::uiCalibration::SIntrinsicEdition`

Depreciate some bundles and services:
- bundles: ioZMQ, uiZMQ, uiNetwork
- services: SProbeMesh and SProbePosition from echoEdSimu

### getObject

*Replace the last 'getObject()' by 'getInOut()'.*

Replace the last two getObject() by getInout().
They were forgotten in service that already use getInout() for these data,
so the deprecated log is useless.

## New features:

### VRRender

*Add activity to upload DICOM series via DicomWeb protocol.*

New activity that anonymizes and uploads DICOM series onto an Orthanc PACS.

### proxyConnection

*Catch exception when the connection failed.*

Catch the exception raised when a connection failed between signals/slots
defined in the configuration.
It displays a error log with the signal/slot information.

### cvIO

*Add new conversion function between cv::Mat and f4s matrices.*

Convert from/to `::cv::Mat`to ::fwData::TransformationMatrix3D
Convert from/to `::cv::Mat`rvec & tvec convention to ::fwData::TransformationMatrix3D
Add new unit tests cases.
Refactor Calibration code to use new helpers.

merge(54-refactor-trasformationmatrix3d-from-to-opencv-mat): into 'dev'

### trackedCameraCalibration

*Merge activities.*

Fuse sense specific activity and rgb activity thanks to SGrabberProxy.

### video

*Import VLC, Orbbec and RealSense grabbers.*

VLC, Orbbec and RealSense grabbers code is now open and imported into fw4pl-ar, as well as the video filtering. The VLC grabber is convenient especially for RTSP streams. It may also be used as a fallback when the QtMultimedia grabber fails... The Orbbec grabber works for Astra camera and the RealSense brings support for cameras based on Intel sensors.

### fwRenderOgre

*Add a helper to convert pixel to view space position.*

The function `convertPixelToViewSpace` translates a pixel coordinates to view space coordinates.

### SNegato2D,3D

*Use the transparency of the transfer function (optionally).*

A new option was added to use the transparency of the transfer function.

### SAxis

*Add a label configurable option.*

SAxis now has an option `label` that can be set to `true` or `false`
to display or hide the axis labels (`true` by default).

### SRender

*Add a 'sync' renderMode.*

In the following of our recent rework of the synchronization for real-time augmented-reality, this new mode allows to make the Ogre generic scene compatible with the approach. The example ExSimpleARCVOgre was reworked to use the new sync mechanism and proves that this works.

## Documentation:

### eigenTools

*Document helper namespace.*

### visuOgreAdaptor

*Update some documentation.*

The documentation of several adaptors were fixed.

## Bug fixes:

### cmake

*Update wildcard to search all external libraries.*

Before only .so.* was found.

### pchServicesOmp

*Remove clang specific hack about OpenMP.*

Remove a clang specific OpenMP hack in our CMake code.

### fwDataCamp

*Fix compilation.*

Add a missing header in fwDataCamp (Build without PCH)

### plugin_config_command

*Support 0 in service or bundle names.*

Fix the regex used to generate the service definition in plugin to
support zero.

### docset

*Unbreak broken docset generation.*

### boost

*Add support of Boost 1.67 on Windows.*

Boost >= 1.67 changes the default random provider on Windows to use BCrypt.
So a link to system library bcrypt is now required to use Boost::UUID.

The changes are compatible with old Boost version.

### ARCalibration

*Remove warnings by using seriesDB key instead of series.*

Fix series keys to seriesDB used in various configurations because it will be removed in 18.0 version of FW4SPL.

### activitySelector

*Remove warnings by using seriesDB key instead of series.*

Fix series keys to seriesDB used in various configurations because it will be removed in 18.0 version of FW4SPL.

### beginnerTraining

*Fix the training samples.*

- fix the documentation for the plugin.xml generation
- remove the fwServicesRegisterMacro from the services to let cmake to generate the right one
- add getAutoConnections() for tuto03 and tuto04 SStringEditor

### fwRenderOgre

*Correct valgrind errors and leaks.*

Memory errors were fixed and memory leaks detected by valgrind (memcheck) on the test suite:
* One out of bounds in read `fwRenderOgre::helper::Mesh`
* Memory leaks on Ogre root destruction

### fwRenderOgre

*Missing headers.*

### fwRenderOgre

*Remove clang specific hack about OpenMP.*

Remove a clang specific OpenMP hack in our CMake code.

### R2VBRenderable

*Clear vertex declaration before filling it.*

This caused the varying to be duplicated, and thus the program link to fail.

### Mesh

*Generate normals each time the mesh is modified.*

For triangle based meshes, when we don't have normals, we generate them. The problem was that it was only done on the first update of the mesh. If points were added to the mesh, the corresponding normals were not computed accordingly, thus the normal layer ended to be be shorter than the position layer. This led eventually to crash at some  point...


# fw4spl 17.0.0

## Bug fixes:

### VRRender

*Do not crash when clicking on the distance button in VR.*

The service `::uiMeasurement::editor::Distance`was also cleaned a bit, and the unused configuration option 'placeInscene' was removed.

### docset

*Generation on case sensitive systems.*

### SMesh

*Lock the input mesh properly in slots.*

### SPoseFrom2d

*Trigger modified signal even if nothing is detected.*

To keep the processing pipeline updated, we need to keep to trigger the modified signal anytime, like in SArucoTracker.

### videoQt/editor

*Properly handle button actions on choose device.*

- Use accept() and reject QtDialog slots instead of our own onValidate() and generic close()
- In SCamera, check the result of exec dialog window to check if it's canceled and don't continue to configure the camera if so.

### Mesh

*Do not compute normals with point based meshes.*

We are not supposed to compute normals when displaying a point based mesh only, however the condition testing this was wrong in the code.

### fwRenderOgre

*Missing headers.*

### material

*Ensure Common.{program,materials} are parsed first.*

Depending on your file system, the `Common.program` could be parsed after the `Video.program`, causing it to fail because it needs `TransferFunction_FP`, which lies inside Common.program to be declared first.

### IDVR

*Compute the countersink geometry in world space.*

We changed the way the MImP IDVR countersink geometry (CSG) is defined/computed:

* CSG used to have a fixed viewport radius, it now has a fixed angle and isn't resized when zooming with the camera.
* Depth lines now start at the importance zone and are in the same unit as the image's spacing.
* The CSG border had to be removed because we couldn't easily adapt it to this new method :crying_cat_face:
* Greyscale CSG and modulation are now separate.

### SMesh

*Build error with GCC 5.4.0.*

### SAxis

*Make the visibility changeable and fix adaptor stop.*

### RayTracingVolumeRenderer

*Do not delay the resize of the viewport.*

Delaying the resize of the entry points textures broke the auto-stereoscopic rendering. This was introduced recently in 6e2946 but was actually not necessary and did not fix anything.

## New features:

### uiPreferences

*Handle floating value in preferences.*

SPreferencesConfiguration only handles integer values.

 - number configuration element is now deprecated it has to be replaced by int
 - add double configuration element to handle float/double type (min: -1000000.0 max:1000000.0, decimals: 6)

### SSignalShortcut

*Create new service to handle shortcuts.*

A new SSignalShortcut service in fw4spl has been added. This service allows to map keys or combination of keys to the trigger of a signal.

### dicom

*Add dicom_reference in Image and Model Series.*

The purpose of this commit is to keep DICOM tags into fw4spl data and use them to create back valid DICOM to save image and/or models.

- Added new example ExDicomSegmentation to generate a ImageSeries mask and a ModelSeries
- Removed `::boost::filesystem::path`in DicomSeries
- Added BufferObject in DicomSeries to store Dicom data
- Updated gdcm/dcmtk reader/writer and unit tests
- Updated `::opImageFilter::SThreshold`to `::fwServices::IOperator`(used in ExDicomSegmentation)
- Updated `::opVTKMesh::SVTKMesher`to `::fwServices::IOperator`(used in ExDicomSegmentation)
- Added dicom_reference in ModelSeries and ImageSeries
- Added new MedicalData version V11

### Calibration

*Add charuco calibration.*

Add ChArUco board calibration in ARCalibration:
* New Bundle with services related to Charuco calibration
* Brand new utility to generate charuco board.
* New activity in ARCalibration
* ARCalibration has been updated to version 0.5
* Both standard calibration and charuco calibration displays now reprojection error when calibration (intrinsic and extrinsic) is performed.
* videoCalibration Bundles were moved from video folder to calibration folder.

### SPoseFrom2D

*Add a points list data containing the corners of the marker model.*

SPoseFrom2D now provides an inout data that can be used to retrieve the
3D geometry of the marker model.

A mistake was also corrected in hand eye calibration, that called `SOpenCVIntrinsic` instead of `SSolvePnP`. In that case, the camera calibration was overwritten by the first service. Now, it just finds the
pose of a chessboard model in the camera only, without calling the camera calibration service (what we really want to).

### MedicalData

*Update fw4spl-ar data to V13AR.*

- This commit adds a new data version V13AR for AR data
- This new V13AR is require to manage new ModelSeries & ImageSeries with Dicom reference (fw4spl!259)

### SLandmarks

*Add adaptor to display landmarks.*

The new adaptor SLandmarks displays landmarks with Ogre generic scene.

### SLine

*Allow length update via a slot.*

A `updateLength()` slot was implemented to update of the length of the rendered line.

## Refactor:

### ioAtoms

*Find the correct version without an XML parameter.*

Improve `::ioAtoms::SReader`and `::ioAtoms::SReader`to find the correct
data version without setting an XML parameter, only the 'patcher' tag is
required to use the patch system.

When no version is defined in SReader and SWriter, the current version
of MedicalData defined in fwMDSemanticPatch is used. This version can be
overridden by the new method 'setCurrentVersion'. You can still define
your own version and context.

### CMakeLists.txt

*Add discovery of additional repositories.*

Setting the CMake variable ADDITIONAL_PROJECTS was tedious and error-prone. Now we explore the folders at the same level of FW4SPL to find extra repositories. Then a CMake option, set to ON by default, is proposed to enable/disable the repository. This will make CMake configuration phase easier than ever !

### deprecated

*Replace getObject by getInput or getInOut.*

- Replace deprecated `getObject()` by `getInput()` or `getInOut()`
- Add deprecated log if the key is not correct in the configuration.
- Set the services `ExternalDataReaderService`, `SInitNewSeries` and `SSeries` as deprecated
- Improve the `FW_DEPRECATED` macros to display the version where the support will be discontinued
- Add a new macro `FW_DEPRECATED_KEY(key, access, version)` to define the correct 'in/inout' key.

All XML configurations have not been updated, so expect to see more [deprecated] mentions in the log. Please fix your application as required.

### SSeriesDBMerger

*Replace getObject.*

### ioAtoms

*Find the correct version without an XML parameter.*

Override the current version of fwMDSemanticPatch to use the AR version.
Clean the useless IO configuration.

### Synchronization

*Improve the synchronization for augmented-reality.*

We reworked the way we synchronize the video frames and the extracted data in real-time.

So far, we have made an extensive use of timelines. First the video grabbers store the frames in timelines. Then we process some algorithms on them and we store all the extracted data (markers, transforms, etc...) in timelines as well. At the end, we rely on ``::videoTools::SFrameMatrixSynchronizer``to pick frames, matrices, etc... at the same timestamp and give these synchronized objects to the renderer, i.e. the generic scene.

However this does not work well. First this is very tedious to work with timelines, since we need to create a dedicated C++ class for each kind of data we want to manage. For some big data, like meshes, we never did it because this would consume too much memory. And some services are simply not well coded and work directly on the data instead of timelines, etc... Eventually, the renderer even screws up the synchronization since all updated objects request the rendering to be done.

So we propose a different approach here. First we restrict the usage of timelines to synchronize video grabbers together, for instance when you
use a camera with multiple sensors or simply several cameras. After that point, all algorithm process the data directly. A new data ``::arData::MarkerMap``is introduced to store a list of markers, since this was the only "data" that only existed in a "timeline" version. To synchronize the results of the algorithms, we propose a new service called `SSignalGate`. This service waits for several signals to be triggered before sending itself a signal, which indicates everyone before is done. This service is typically used to inform the renderer that it must send everything to the GPU right now. To achieve this, we introduced a new rendering mode in `::fwRenderVTK::SRender`.

You can try Ex04SimpleARCV which uses the new design, but for now everything is backward compatible. But we strongly encourage to have a look at this
very soon and try to port your application to benefit of this improvement.

### SExportWithSeriesDB

*Remove getObject.*

### MesherActivity

*Refactor CGogn and VTK mesher.*

- This commit removed dependency to bundle opVTKMesh in opPOCMesher
- CGoGNMesher is now a standard ::fwServices::IOperator
- Updated MesherActivity config with new VTK/CGoGN mesher API

Refactor MesherActivity

### Interactors

*Allow adaptors to be an interactor implementation.*

This is a first step in the refactor of interactors. We plan to implement interactors directly instead of using the only one SInteractorStyle that instantiates sub-classes. It is actually more complicated than if the interactor does the job directly.

In ARPerfusion, we had to create a new interactor to select regions (ARPerfusion!10). We wanted to implement it as an adaptor, which allows us to test if the design works. So we modify the inheritance to allow adaptors to behave as an interactor directly. Consider this as a temporary step in the migration of interactors, where both solutions are possible.

Besides this, there are some changes that might seem unrelated but they were necessary for our new interactor. There is a first fix to allow all kind of meshes to be displayed with a SPointList adaptor. Then there is a second commit to fix the cell color textures, which were not correctly fetched from the texture used to store them.



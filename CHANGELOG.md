# sight 20.2.0

## New features:

### graphics

*Add a new picker to really pick the current slice image.*

- When using an Ogre picker on a medical image, the picked position does not correspond to a voxel position of the image. In fact, the Ogre negato display the image between \[0;size\] and the picker needs to pick between \[0;size\].

*Add Qt3D SMaterial adaptor.*

Implement qt3d material service:

* `::fwRenderQt3D`::data::Material``object handling a qt3d material
* `::fwRenderQt3D`::techniques::Lighting``object handling a qt3d technique with shader programs to compute lighting mode and rendering options such as point/line/surface rendering or normals visualization
* `::fwRenderQt3D`::SMaterial``adaptor used to create a qt3d material from sight data and attach it to the render service

### io

*Read tfp TF from directory with ::uiTF::SMultipleTF.*

*Add setNotifyInterval method to the Graber interface.*

Adds an option in the videoQt Player to change the notifyInterval option. This is useful to require frame through "setPosition".

*Add birth date request field for PACS query editor.*

### ui

*Allow to manage opacity of the TF in TF editors.*

Allow to manage a whole TF opacity at the same time from the TF editor.

## Refactor:

### core

*SpyLog rework.*

SpyLog has been unnecessarily complex for a long time. In this rework, we propose to:
- deprecate the loglevel `TRACE`. Only `SLM_TRACE_FUNC()` remains, but its occurrences should never be committed.
- deprecate `OSLM_*` macros in favor of `SLM_*` macros, which now take stringstreams as input (no performance penalty)
- all loglevels are now always compiled, which means that the big bloat of `SPYLOG_*` CMake variables were removed.
- occurrences of `OSLM_*` macros were replaced by `SLM_*` macros
- occurrences of `O?SLM_TRACE*` macros were removed or replaced by higher levels logging macros
- the default displayed log level is now `WARN`
- the path of files displayed in the output was shortened to keep the minimum information needed: namespace(s), source fil

*Deprecate configureWithIHM.*

Deprecate `configureWithIHM` and use `openLocationDialog`, backward compatibility is kept until sight 22.0

*Deprecate *dataReg modules and move *dataCamp content to *data.*

This deprecates the usage of dataReg and arDataReg modules, which were so far mandatory as `REQUIREMENTS` for any XML configuration using data. This was done in several steps:

1. a function was added in `fwRuntime` to allow the loading of any regular shared library (we could only load libraries from modules before).
2. `AppConfigManager` was modified to guess the library name when parsing configuration, and load the library accordingly
3. `dataReg` and `arDataReg` were emptied from the hacky symbols, and deprecated
4. the only real useful code, i.e. the XML data parsers from `dataReg` were moved to `fwServices`, so that we can remove the whole module later.
5. `dataReg` and `arDataReg` were replaced by `fwData`, `fwMedData` and `arData` in `Properties.cmake` files
6. `dataReg` and `arDataReg` were removed from all `plugin.xml` requirements.
7. all `*DataCamp` libraries were deprecated and most of their content imported in the corresponding `*Data` libraries. They were linked in an application thanks to the dataReg modules and hacky symbols. I could have loaded the modules manually like data libraries but I think it is simpler to gather everything related to a data into a single library. There is no real use case in my mind where we want only the data without any introspection. And we do not need to plan for a coexisting alternative to perform the introspection, we are anyway too tight with Camp.

This is not a breaking change and it is still possible to keep the deprecated modules.

### graphics

*Deprecate VTK.*

* Deprecates VTk generic scene
* Updates all samples to use Ogre
* Cleans all sample files (xml/hpp/cpp/cmake)
* Fixes some errors in our samples
* Adds a new module `visuBasic` for basic generics scenes
* Rename samples

### io

*Deprecate VLC.*

Deprecation of `videoVLC` since VLC package will no longer be available with new conan system (sight 21.0).

### vision

*Deprecate the bundle.*

Deprecate the bundle `uiHandEye`

## Enhancement:

### core

*Complete meshFunction baricentric  API.*

Add a method `isInsideThetrahedron( const ::glm::dvec4 barycentricCoord);` in the MeshFunction.

*Add [[nodiscard]] attribute for weak/shared_ptr::lock().*

Adds `[[nodiscard]]` attributes in weak_ptr and shared_ptr lock() function. This avoids using lock() as lock function and force user to use returned lock_ptr object

*Update conan.cmake file to v0.15".*

Update conan.cmake file to v0.15.

### graphics

*Improve scene resetting.*

### io

*Increase the DICOM services log level.*

Add detailed logs for our communications with the PACS.

*Improve PACS research fields.*

* Make research fields of ioPacs case insensitive.
* Improve date research.
* Avoid to read unreadable modalities with the series puller.

*Execute PACS queries from a worker.*

``::ioPacs::SeriesPuller``now retrieves series in a worker to avoid the app to freeze.

*Improve ioPacs queries.*

Adds a PACS series selection to `OgreViewer`.

* `SPacsConfiguationEditor`: send notifications.
* `NotificationDialog`: use the height of the notification instead of the width for computation.
* `SSelector`: remove margin.
* `SNegato2D` and `SNegato3D`: avoid a division by 0.
* `Utils`: sometime the default texture as the same size than the image, so there is never initialized.

### ui

*Rename bad named functions.*

* Deprecated `::fwGui::dialog::MessageDialog::showNotificationDialog()` and propose a new `::fwGui::dialog::MessageDialog::show()`
* Deprecated `::fwGui::dialog::NotificationDialog::showNotificationDialog()` and propose a new `::fwGui::dialog::NotificationDialog::show()`
* Renamed all occurrences.

## Bug fixes:

### build

*Add deps between targets and their PCH for Unix Makefiles.*

This fixes errors when building using the Unix Makefiles generator and multiple processes.

### core

*Load library is broken when launching activity using wizard.*

Adds `loadLibrary` method in `new.cpp` file in order to load libraries before starting `AppConfigManager` or `ActivityWizard`

### io

*Cannot save json on mounted disk.*

Create a custom `rename` function that will first try a `std`::filesystem::rename``and then fallback to a basic copy-remove scenario.

Using this avoid the `Invalid cross-device link` error, when saving a json(z) file on a another disk/volume.

*Tf names without extension are not saved.*

### ui

*Enable the current activity of the sequencer if no data are required.*

* Enable the current activity of the sequencer if it's needed.
* Avoid double start of the config launcher.

*The TF editor suppresses points.*

### vision

*Display left and right view vertically.*

Until a better fix, display left and right camera vertically in extrinsic calibration. Doing this provide a better width view of video.


# sight 20.1.0

## Bug fixes:

### build

*Remove ".sh" extension from launcher scripts.*

Both build and install targets are impacted.

Adds `.bin` extension to executable target such as utilities/tests or fwlauncher to distinguish executable targets from shell scripts launcher

*Ninja install issue on linux.*

Test if the install directory ( `CMAKE_INSTALL_PREFIX`) is empty, if not print a warning at configure time.

### ci

*Build on master branch on Windows.*

Fix quoting of branch name while cloning

### core

*Crashes when closing OgreViewer.*

OgreViewer crash at closing, this crash happens because the action `openSeriesDBAct` is working on a thread.

Now we wait until services are properly stopped after `service->stop()` in XXRegistrar class.

*Replace misleading error logs in SlotBase.*

When trying to call/run a slot, an error log was displayed if the signatures don't match, but it is allowed to call a slot with less parameters.
Now, an improved information message is displayed if the slot cannot be launched with the given parameters, then we try to call the slot removing the last parameter. The error is only raised if the slot cannot be called without parameter.

### graphics

*Fix screen selection for Ogre scene.*

Enable the full screen of Ogre scene on the right monitor.

For more informations: https://stackoverflow.com/questions/3203095/display-window-full-screen-on-secondary-monitor-using-qt.

*Slice outline colors are inverted between axial and sagittal in OgreViewer.*

Use blue outline color for Ogre's sagittal view, and red outline color the axial one.

### io

*Change the overwrite method when saving using ioAtoms.*

Change the overwrite method of `ioAtoms::SWriter`. Previously, when a file was overwritten, it was deleted then saved. But if the saving failed, the old one was still deleted.

Now, the file is saved with a temporary name, then the old one is deleted and the temporary name is renamed with the real one.

*Realsense doesn't work on latest 5.4 linux kernel.*

Update the version of librealsense package to latest (2.35.2). This fix issue of using realsense camera on linux kernel > 5.3

*Remove useless requirement in ogreConfig.*

Remove `ioVTK` module from `ogreConfig`. No reader, writer or selector was used in the configurations.

*Change realsense camera timestamp to milliseconds.*

Correct the Realsense grabber timestamp from microseconds to milliseconds

*Allow to re-start reading a csv without re-selecting the file path.*

Update 'SMatricesReader' to properly close the file stream when calling `stopReading` slot and clear the timeline.
Properly stop the worker to avoid a crash when closing the application.
Also remove the default delay of 1s when using one shot mode.

*Remove hard-coded codec and only offer mp4 as extension available in SVideoWriter.*

* Limits extension selection to only `.mp4` for now.
* Limits FOURCC codec to `avc1`. This codec is also linked to the `.mp4` extension.

*Fix a crash with OpenCV grabber.*

- Start/stop the worker of OpenCV grabber when service start/stop

### ui

*Notifications crashes when no active window was found.*

Avoid crash of app when notifications were displayed without active window (focus lost).
If no active window is found, print an error message and discard the current notification.

*Prevent dead lock when opening organ manager.*

Block Qt signals in SModelSeriesList when changing the state of the "hide all organs" checkbox.

## Enhancement:

### ci

*Re-enable slow tests and remove ITKRegistrationTest.*

### core

*Add empty constructor to Array iterator.*

* Add empty constructor to Array BaseIterator, to make it compatible with std::minmax_element

### doc

*Improve IService doxygen about output management.*

Describe how to properly remove the service's outputs and why and when we should do it.

Add a check when the service is unregistered and destroyed to verify if it owns an output that will also be destroyed (if the output pointer is only maintained by this service). Currently only an error log is displayed, but it may be replaced by an assert later. Other services may work on this object but didn't maintained the pointer (they used a weak pointer).

*Update the screenshots in README.md.*

Update README.md with more up-to-date screenshots/animations

### graphics

*Use ogre to display videos in ARCalibration.*

* Use Ogre for `ARCalibration`.
* Properly clean ogre resources before root deletion.

*Manage default visibility of `SNegato3D` from XML configuration.*

Call `setVisible` method at the end of the starting method.

### io

*Improve ioPacs module.*

* Improves connection/disconnection management.
* Improves the UI and request forms.
* Use new disconnection method of dcmtk.
* Fills new `medData` attributes.

### ui

*Add goTo channel in ActivityLauncher.*

Add `goToChannel`in `ActivityLauncher.xml` in order to call `goTo` slot from an activity.

*Replace Ircad and Ihu logos by the unique Ircad/Ihu logo in the sequencer view.*

Modify ActivityLauncher configuration to replace Ircad and Ihu logos by the new version of Ircad/Ihu logo.

*Improve assert messages in fwGui.*

Add as much information as possible in the assertion messages: service uid, why the error occurs, ...

*Add check/uncheck slot in SSignalButton.*

These two new slots allow to easily check/uncheck the button without using a boolean parameter. They can be connected to a SSlotCaller or any signal.

Update TutoGui to show an example with these two new slots.

*Allow to delete all reconstructions or one from a right click.*

- Allow to delete all reconstructions from `SModelSeriesList`
- Delete specific reconstruction from a right click.

*Improve Qt sequencer colors management.*

Allow to manage more colors in the Qt sequencer.

## New features:

### core

*Implements copy for mesh iterators.*

Implement `operator=()` and `operator==()` on mesh iterator's internal structs (PointInfo and CellInfo).
It allows to use `std`::copy``(or other std algorithms) with Mesh iterators.
Update Mesh unit tests to check using `std::copy`, `std`::equal``and `std::fill`.

### graphics

*Add new experimental Qt3D renderer.*

A new experimental renderer based on Qt3D has been integrated, providing basic support for mesh visualization.

Three samples are provided to show how Qt3D could be integrated into a classic XML application, a pure c++ application
or a QML application:
* TutoSceneQt3D
* TutoSceneQt3DCpp
* TutoSceneQt3DQml

The following classes and services have been introduced:
* `::fwRenderQt3D`::GenericScene``object handling a qt3d scene
* `::fwRenderQt3D`::FrameGraph``object to attach to a GenericScene, allowing to define custom qt3d framegraphs
* `::fwRenderQt3D`::SRender``service used to define a GenericScene object within Sight context and attach adaptors to it
* `::fwRenderQt3D`::Mesh``object creating a mesh from sight data using qt3d geometry renderer
* `::fwRenderQt3D`::IAdaptor``class providing base functionalities for Qt3D adaptors.
* `::visuQt3DAdaptor`::SMesh``adaptor used to create a qt3d mesh from sight data and attach it to the render service

*Improve Ogre's SLandmarks for 2D scenes.*

Improve Ogre resources management in '::visuOgreAdaptor::SLandmark' and display the landmarks in 2D negato scenes only on the slice of the landmark.

*Add Ircad-IHU logo overlay in Ogre Scene.*

- 8 positions are supported: left-top, left-center, left-bottom, center-top, center-bottom, right-top, right-center and rght-bottom.

### io

*Allow to save and load multiple TF with TF editor.*

* Add new buttons to export and import a TF pool to `SMultipleTF`
* Use the new API in `SMultipleTF`.

## Refactor:

### core

*Manage pixel format in FrameTL.*

Manage pixel formats: Gray Scale, RGB, BGR, RGBA and BGRA.
Now, to initialize the frame timeline, the format must be specified, the previous initialization method (with number of components)is deprecated.
Also improve the unit tests to check timeline initialization and copy.

Update `SFrameMatrixSynchronizer`: use the new pixel format to initialize the image, but still support the old API when the format is undefined. Also use the new service's API with locked and weak pointers.

### graphics

*Rename the layer "depth" to "order".*

Replace `depth` attribute of Ogre layer by `order`.

### ui

*Factorize visibility slots in Ogre3D renderer.*

- Factorise `updateVisibility`, `toggleVisibility`, `show` and `hide`. `setVisible` is the only method to reimplement in subclasses.


# sight 20.0.0

## Bug fixes:

### .gitlab

*Improve sheldon stage.*

Use sheldon and build the project on the merge result instead of the working branche.

*Improve sheldon stage.*

### activities

*Split bundle to separate gui services from core services.*

Separate service using Qt Widget from other services in order to use generic service with a Qml application, because Qt widgets cannot be instantiated in a Qml application.

A new bundle `uiActivitiesQt` was created where `SCreateActivity` and `SActivityLauncher` has been moved. The old `SCreateActivity` and `SActivityLauncher` services has been kept in activities but set as deprecated. A fatal would be raised if you use them in a Qml application.

`ENABLE_QML_APPLICATION` variable has been added in the main CMakeList to define if there is a Qml application that will be launched. It is temporary because we need to keep the old API until sight 21.0 and allow to remove the dependency to Qt Widgets in activities bundle.

### ActivityLauncher

*Fix reader/writer configuration.*

Fix the reader and writer configurations used by `ActivityLauncher` config: use the configuration given in parameters instead of the default one.

### AppConfig

*Fix XML variable regex.*

The regex is used to know if the xml attribute contains a variable (`${...}`). But in a few cases, for example in signal/slot connections, the variable is followed by other chars. For these cases, the regex should not be limited to `${...}`but to `${...}....`.
The unit tests have been updated to check this error.

### AppConfigManager

*Fix useless variable.*

Remove wrong `#ifdef` arround `FwCoreNotUsedMacro(ret);`

### Array

*Implement missing copy constructor in Array iterator.*

Implement copy constructor for const iterator and assignment operator.

### build

*Fix build on linux with gcc7 in release mode.*

- fixed sheldon coding style and define in fwCom
- fixed unused variable (release mode) in fwServices

### CalibrationInfoReader

*Wrong image channel order.*

* Convert to the sight's default color format when reading calibration inputs.
* Sort the calibration input by their filename.
* Update the related tests.

### CI

*Disable slow tests until they all pass.*

*Sheldon-mr job doesn't work.*

According to #402, the sheldon-mr job seems to fail when merge commit are present in-between dev and the current mr branch.

Here the `git merge-base` function has been replaced by a diff of revision list between the MR branch and dev, and then keep the oldest common ancestor.

**Explanations**

* `git rev-list --first-parent origin/${CI_COMMIT_REF_NAME}`: revision list of the MR branch
* `git rev-list --first-parent origin/dev`: revision list of dev (/!\\ we need to check is there is a limitation on number)
* `diff -u <(git rev-list --first-parent origin/${CI_COMMIT_REF_NAME}) <(git rev-list --first-parent dev)` : basic diff between the two list
* `| sed -ne 's/^ //p'`: removes the diff line ("+" "-", etc), to keep only the common revisions
* `| head -1`: keep the first form common revisions.

This is, indeed, a bit over-complicated, and can be simplified by a simple merge on the CI side, and then run sheldon, but this avoid any potential merge conflicts checking beforehand.

*Use CI_JOB_TOKEN when cloning sight-data.*

### CMake

*Disable RelWithDebInfo build type.*

*Resolve Qt plugins path in SDK mode.*

Do not install the qt.conf from our Conan package that points to a bad plugin location. This overrides the location set in `WorkerQt.cpp`, preventing Qt application built with the SDK from launching.

Besides this, I also fixed file permissions of installed files (executable were not actually executable) and I corrected a warning at the end of a CMake run because of a wrong usage of `PARENT_SCOPE` in the macro `generic_install()`.

*Resolve missing export when using SDK mode.*

The systematic build of the object library broke the SDK build, because the export was missing for the object library. However, this means users would have to link for instance against `fwCore_SHARED_LIB` instead of `fwCore` which is the object library. This is counter-intuitive and really not desirable.

We chose to invert the logic. `fwCore` is now the shared library and `fwCore_obj` is the object library. This solves the problem for external users but not for sight developers who would need to modify all `CMakeLists.txt` to link fwCore_obj with 3rd party libraries instead of fwCore. We found a middle-ground by building the object library **only** when the variable GENERATE_OBJECT_LIB is set. This variable was set for `fwRuntime` to enable the build of `fwRuntimeDetailTest`. Its `CMakeLists.txt` was last modified to link against `fwRuntime_obj` instead of `fwRuntime`.

On top of that, some corrections were made because of the rework of `fwRuntime` and the usage of `std::filesystem`.

*Conan cmake script not correctly downloaded.*

- Add check hash of `conan.cmake` file for:
  * avoid downloading if the file already exists and is valid
  * download the file if it's corrupted

*Add cmake 3.17.0 support.*

Add cmake 3.17.0 support

*Fix the plugin config command.*

Include the appropriate headers in generated `registerService` files.

*Strip cmake compilation flags on Windows.*

Avoids cmake adding a space at the end of compilation flags

*Fix executable installer.*

This enables the packaging of executable programs under windows.

By the way, the call of generic_install() is now done automatically for all APP and EXECUTABLE targets, thus enabling packaging automatically. A warning is displayed if the macro is called in the CMakeLists.txt of the application.

### conan-deps

*Update hybrid_marker_tracker version.*

Update hybrid_marker_tracker library version that fixes memory leak.

### ConfigLauncher

*Add existing deferred objects to configuration.*

* Notify xml sub-configurations that optional objects already exist.
* Deprecate `IAppConfigManager`.
* Improve the documentation of modified classes.

### dataReg

*Add missing include to compile with latest VS2019 16.6.0.*

### Dispatcher

*Fix wrong mapping for uint8 type.*

`isMapping` method is used by Dispatcher to find the right type for calling template functor.

### doxygen

*Doxygen uses the correct README.md to generate the main page.*

Update Doxyfile to fix the path of the main README.md. Now the doxygen is generated using the main README file as the main page.

### ExIgtl

*Add auto-connections INetworkSender.*

* Add missing auto-connections method in INetworkSender
* Fix ExIgtl example
* Add JetBrains IDEs (CLion) project folder in git ignore file

### ExImageMix

*Fix image voxel information.*

Remove the useless hack that skipped some events in VTK picker interactor, so the information was not up-to-date.

### ExSimpleARCVOgre

*Add missing media bundle.*

This solves the crash of ExSimpleARCVOgre at start. The bundle media was missing.

### fwData/Image

*Deep copy.*

* Fixes an assert raised in debug when the image is locked and deep copied.
* Don't let the data array expire.
* Add a test to ensure this doesn't happen again.
* Fix ExDump.

### fwGdcmIOTest

*Test fails.*

Fixes fwGdcmIOTest by using correct type when setting Tags Values.

* ConsultingPhysicianName is a list
* Patient Size/Weight/MassIndex are double
* Attributes was missing when copying a Study

### fwGuiQt

*Allow notifications to work with ogre scenes.*

Fixes notifications for app that used Ogre scenes.
Moves the dialog according to the parent.

### fwLauncher

*Allow to exit when receiving signals.*

- Use regular exit() to stop program execution when receiving SIGINT, SIGTERM, SIGQUIT signals.

### fwMath

*Use glm vector when computing barycenters and fix mistake.*

* Replace all `fwVec3` structure by `::glm::dvec3`, along with glm maths functions.
* Fix mistake in dot product when computing barycentric coordinates.
* Update unit-test

### fwRenderOgre

*Fix query flags and ray mask.*

* Sets all pickable adaptors query flags to a default value ('::Ogre::SceneManager::ENTITY_TYPE_MASK').
* Sets pickers query mask to a default value ('::Ogre::SceneManager::ENTITY_TYPE_MASK').
* Query flags on adaptors are now sets, (previously, they was added).
* Updates documentations on updated files.

It allows pickers to pick all adaptors by default. This can be adjusted in xml by modifying the query mask of the picker, and the query flags of adaptors

### fwVtkIO

*Fix warnings (as error) when building with Clang.*

### gitlab

*Improve sheldon stage.*

Fix the build on dev, master and tags

### HybridMarkerTracking

*Fix tracking issues with strong distortion.*

Removing re-definition of member variable and undistort displayed image allows to correctly re-project tracking position on videos with strong distortions.

### IActivitySequencer

*Retrieve more data on update.*

The sequencer allows to store all data related to all activities in a member called `requirements`. When the sequencer checks an activity for the first time, it'll create it and add all it's needed data that already exist in the `requirement` list into the activity composite.

If we have two activities `A1` and `A2`, `A1` needs an image called `CT` and `A2` needs the same image, and also a matrix `MA` created by `A1`.

You launch A1 after adding its needed data in the wizard (`CT`), at this time, the sequencer stores `CT` in its `requirements` list.

So A1 is open, the sequencer will check if needed data for A2 have been loaded (Allows to enable a button in the view). To do that, `A2` is created and the sequencer adds the `CT` into the composite of `A2`, but `MA` is missing, nothing is launched.

Now `A1` has created the matrix `MA` and asks the sequencer to open the next activity, `A2`. The sequencer checks needed data, but `A2` is created, and the sequencer only updates data that are contained in the composite of `A2`.

* These modifications allow to update data and also to add potential new data generated by a previous activity (`IActivitySequencer::getActivity`).
* Now, before checking a next activity, the sequencer stores data of the current one (the current one has maybe created a new data).
* `IActivitySequencer`::parseActivities``returns the last valid activity. Before modifications, the next one was launched.

### Image

*Fix the iterator when using a different type than the image.*

The image iterator is now computed based on the format of the iterator instead of using the image type. You can now use Iterator on int16 image. It can be useful to fill the image with zeros. You can also parse int32 image with int64 iterator to gain performance.
`SLM_WARN` have been added when you use an iterator on a different type.

### ioDcmtk

*Fix the series DB reader and the CI last modification time of file.*

### ioIGTL

*Fix the module loading on windows.*

- Moves the module `ioNetwork` to SrcLib, it only contains interface. Furthermore some modules like `ioIGTL` links with it, and link between modules are prohibited.

### ioVTK

*Vtk Mesh readers and writers only support VTK Legacy files format.*

Add more Mesh format in VTK readers/writers.

* VTK Polydata (\*.vtp)
* Wavefront OBJ (\*.obj)
* Stereo lithography format (\*.stl)
* Polygonal File Format (\*.ply)

This was added to improve compatibility with other VTK sofwares (paraview for example), it seems that the \*.vtk format is a legacy format and we should use \*.vtp when dealing with polydata (our ::fwData::Mesh).

**Note:** when using OBJ/PLY/STL format you may loose some informations (point normals/point color/...), often it saves only global geometry of the data (points, edges, ...).

### IParameter

*Add missing makeCurrent in IParameter.*

Add missing `makeCurrent` in `fwRenderOgre/IParameter`

### ModelSeries

*Add const in model series helper.*

Update `::fwMedDataTools::ModelSeries`to add const on shared pointer parameters. It allows to call the helper with const sptr.

### OgreVolumeRendering

*Fix the ambient occlusion.*

* Fix a crash in `SummedAreaTable` due to bad storage (when `m sat Size[0]` is 256, the old `std:uint8_t` stores 0).
* Fix a crash when the software is closed.

### openVSlamIOTest

*Add missing dependencies for link.*

This adds ffmpeg and Qt as explicit dependencies. There is clearly a bug in the OpenCV target but until this is fixed upstream, we can use this solution.

### openvslamTracker

*Do not download the vocabulary file each time.*

Check the file hash to prevent it to be downloaded on each CMake configuring step.

### RayTracingVolumeRenderer

*Correctly blend the volume with the scene behind it.*

* Fix alpha blending between the volume and the scene behind it.
* Fix z-fighting between the negato picking widget and the negato plane when OIT is enabled.
* Fix z-fighting between the volume and the negato planes.

### SActivitySequencer

*Qml import path is not properly defined.*

Check if a `qml` folder exist on the application root folder (for installed application) and use the conan path otherwise.

### SArucoTracker

*Doesn't display marker if image components < 4.*

Test the number of components of the input image before doing OpenCV conversion.

With this modification we can now handle both 3 and 4 components images.

### SCalibrationInfoReader

*Don't convert the image if loading failed.*

Fix a crash when loading a folder containing files not readable as images.

### SeriesDBReader

*Skip structured report and improve the activity data view.*

When you load a DICOM folder that contains a `SR` modality with the reader ``::vtk`Gdcm`::Series`FBReader`, it throws an execption. Now it's skipped with a message.

The activity data view is also improved in this MR.

### SFrameMatrixSynchronizer

*Use new image API.*

* Fix the issue when the number of components are not properly reset (#423)
* Add a missing write lock on matrices.
* Use the new image API

### SFrameWriter

*Add missing else statement.*

### Sight

*Fail to compile with Clang.*

This adds the AES flag for Clang, allowing to build fwZip successfully.

Initially, the MR was opened to fix this issue but actually many targets failed to compile with Clang, thus other minor fixes are included.

### SMatricesReader

*Fix default path.*

Fix the default path in `::ioTimeline::SMatricesReader`. The default path must be a folder, so we use the parent path of the selected file to set the default reader path. It allows to re-open the file dialog in the same folder.

### SMatrixWriter

*Add fixed floating point format and set float precision.*

Set floating precision fixed to 7 digits.

### SMesh

*Fix recursive read lock.*

Update ``::visuOgreAdaptor::SMesh``to remove a read lock in an internal method and only call it in the slots. It fixes a random freeze when starting the adaptor in a large application.

*Fix auto reset in mesh adaptor.*

Fix auto reset camera in `::visuOgreAdaptor::SMesh`:

* call auto reset when the vertices are modified
* call request render when the adaptor is started

### SNegato2DCamera

*Modify camera position in function of the image.*

Modify the AutoReset camera method to reset the camera position in function of the image and not the world position.
If there is other adaptors than SNegato2D, the scene will be autoreset with the position of the image. It avoids some strange scalling due to other adaptors.

### SNegato3D

*Properly use the ITransformable interface.*

Properly use the ITransformable interface in SNegato3D.

### spylog

*Reduce usage of FW_DEPRECATED Macro.*

Small cleanup of usage of `FW_DEPRECATED` macros.

* Macros was removed from widely used functions
* keyword `[[deprecated]]` was added where it was missing

### SText

*Fix text configuration.*

The `text` configuration was wrong when parsing xml file.

### STransferFunction

*Avoid assert when icons path are not initialized.*

- Fix crash when icons path are not initialized in `StransferFunction` service.
- When icons path were not defined, we have this assert: `Assertion 'path.is_relative()' failed. Path should be relative`

### STransform

*Remove or add the node instead of change the visibility.*

Show or hide the transform node directly impact attached entities and services like `::visuOgreAdaptor::SMesh`, that can't manage its visibility since STransform change it any time.
Now, the node is juste remove or add from it's parent to avoid a visibility error.

### STransformEditor

*Add missing lock.*

Add missing mutex lock in `uiVisuQt::STransformEditor`

### SVector

*Fix the visibility update of SVector.*
* Add two slots to show and hide the vector

### SVideoWriter

*Computes video framerate.*

* Removes the default framerate value hardcoded to 30 fps in SVideoWriter
* Computes framerate using the timestamps of the first frames in the timeline

### SVolumeRender

*Fix usage of transfer functions.*

* Create a new service `uiTF`::STransferFunction``instead of `uiTF`::TransferFunctionEditor``(it also fix the swapping method and lock data).
* Improve the TF usage in `visuOgreAdaptor::SVolumeRender`.
* Set the sampling rate at starting of `visuOgreAdaptor::SVolumeRender`.
* Fix the `GridProxyGeometry` by avoid uniforms sharing between programs.
* Fix the `makeCurrent` method.

*Fix a crash with the volume render.*

*Double lock when updating the volume sampling.*

### test

*IoVTKTest randomly crashes.*

Fix ioVTKTest random crash. Sometimes writer doesn't load the reconstructions in the same order than the reader saved it.

We need to force writer to load file in a specific order:

* prefix reconstruction filename by its index in reconstructionDB (like "0_Liver")
* sort filenames by alphabetical order, using the previous prefixed index, to ensure that reconstructions are loaded in the same order than the generated ones.
* add messages in CPPUNIT assert marco to help to find failing tests.

### TutoTrianConverterCtrl

*Console application cannot be launched with parameters.*

Forward arguments of .bat/.sh scripts to fwLauncher.

### videoVLC

*Fix VLC plugins dir.*

* use current working path to find VLC plugins folder in `videoVLC` for installed applications
* fix cmake install target for apps using libvlc:
  * on Windows (with MSVC2019) VLC is now manually installed to avoid `fatal error LNK1328: missing string table` in fixup_bundle
  * on linux vlc plugins path is now analysed by the fixup_bundle script to find required dependencies
* update VLC conan package to the version 3.0.6-r5

### visuOgreAdaptor

*Properly generate the r2vb in SMesh.*

### visuOgreQt

*Intel mesa regressions.*

* Fix regressions when using intel mesa GPU following the context handling MR (!249)
* Handle buffer swapping manually instead of relying on ogre.

*Remove unused variables.*

* Fixes a warning on the CI preventing sight from building.
* Removes an unused parameter in `visuOgreQt::Window`.

*Always force rendering when the window is exposed.*

* Fix the call to `renderNow` in `visuOgreQt/Window`
* Request render when changing visibility parameters in `ExSimpleARCVOgre`

*Send mouse modifiers as keyboard events.*

* Take mouse modifiers into account for mouse events.
* This is a quick fix and should be refactored later on.

## Documentation:

### fwData

*Improve doxygen of Image, Mesh and Array.*

* add examples, fix some mistakes.
* improve `Image::at(x, y, z, c)` to add the component parameter, it allows to retrieves the value of an element of a image with components

### README

*Remove broken install links.*

Remove broken links pointing to old documentation pages.

### visuOgreQt

*Clean the bundle.*

Cleans the `visuOgreQt` bundle.

## Refactor:

### Boost

*Replace boost use by C++17 new features.*

Replace usage of Boost by **standard library** versions of:
- `boost::filesystem`
- `boost::make_unique`
- `boost::any`
- `boost`::assign``(most of them, some were left because of specific boost containers such as bimaps)

Also, `fwQtTest` failed a lot during testing, so a fix has been proposed. It should no longer fail now.

### CMake

*Rename THOROUGH_DEBUG option.*

Rename `THOROUG_DEBUG` into `SIGHT_ENABLE_FULL_DEBUG`

### deprecated

*Remove last remaining deprecated classes and methods for sight 20.0.*

* Remove deprecated class Bookmarks
* Remove deprecated class AttachmentSeries and the associated reader and converter class (atom patches have been kept to support the loading of old data).
  * it implies the increment of MedicalData version to V14 and V16AR
* Remove the stop of the worker in its destructor
  * it implies to properly stop the workers before to clear the worker registry (::fwThread::ActiveWorkers). In fact, the workers should be stopped by the classes that created them, but as the API to remove a worker from the registry does not exist, it will be done in #521.
  * it also implies to stop manually the workers used in the unit tests and not managed by the registry

*Remove the use of deprecated Image, Mesh and Array API.*

* Remove the last remaining usage of the deprecated API of Image, Array and Mesh outside of fwData and fwDataTools: add a `2` after get/setSize, get/setOrigin, get/SetSpacing.
* Fix dump lock on Image and Arry iterators: lock bust be called before accessing the buffer.
* Improve the documentation about the dump lock.

*Use new Image, Mesh and Array API.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* refactor `igtlProtocol` library to use new API
* refactor `fwCommand` library to use new API
* remove useless helpers includes
* fix MeshIterator: set 'operator=()' as virtual and remove redundant cast

### filetree

*Rename root folders.*

All root folders were renamed, and first of all, Bundles into modules. We also chose to stick to lower-case only and so we renamed all existing folders to their lower counterpart. Besides this SrcLib was shortened into libs and Utilities in utils.

Here was the old file tree:

| Apps
| Bundles
| CMake
| fwlauncher
| Samples
 \ Examples
  | PoC
  | Tutorials
| SrcLib
| Utilities

And here is the new one:

| apps
| cmake
| fwlauncher
| libs
| modules
| samples
 \ examples
  | poc
  | tutorials
| utils

### fwData

*Clean fwData and fwTest with new API of Image, Mesh and Array.*

* Update `config.hpp.in` to add `<PROJECT_NAME>_DEPRECATED_CLASS_API`, it allows to set the `deprecated` attribut on a class (display a compilation warning).
* Update `fwTest` and `fwData` libraries to remove the dependency to `fwDataTools`
* Clean `fwDataTools` to remove the use of deprecated helpers, but the helpers are still here.
* fix SImagesBlend adaptor: fix size, spacing and origin comparison

*Use new Image, Mesh and Array API in uiXXX Bundles.*

* Use new Image, Mesh and Array API in uiXXX bundles
* Improve `ioVTK` and `fwVTKIO` unit tests
* Fix `fwTest` Image generator (number of components was not properly set)
* Fix `::fwData::Image`, add a missing const on `getPixelAsString` method

*Improve the API of `::fwData::Array`.*

Simplifies Array API by moving the methods from the helper to the data itself:

* deprecate `::fwDataTools::helper::Array`
* integrate the useful methods from the helper to `::fwData::Array`
* deprecate the component attribute
* add `Iterator<Type>` and `ConstIterator<Type>` to iterate through the array buffer

### fwDataTools

*Use the new Mesh API.*

* Refactor Mesh iterator to access values on a const variable.
* Use the new Mesh API in ``::fwDataTools::Mesh``to compute normals and improve unit tests.
  * Also fix the normals: when the cell contains more than 3 points, only the last 3 points were used to compute the normal.
* Improve Image ans Array documentation.

### fwRenderOgre

*Use the new API for Mesh, Image and Array.*

Refactor `fwRenderOgre` and `uiVisuOgre` to use new Image, Array and Mesh API.

*Clean the media folder.*

Sorts glsl, metarial and compostior.

### fwRuntime

*Remove usage of deprecated methods.*

* This removes the usage of all deprecated methods by !282
* This removes the usage of the term `bundle` in local variables, private functions, comments.

*Rename 'Bundle' into 'Module'".*

The first intent of this MR is to rename the term 'Bundle' into 'Module'. As stated in #404, 'Bundle' is not a term widely spread in software for the meaning we give to it. More often, software use either the term 'Module' or 'Plugin'. Late in summer 2019, we decided to choose 'Module'.

However we did not want that change to be a breaking change. So as usual, during two major versions, 'Bundle' and 'Module' terms are likely to coexist and the old API should be deprecated but maintained. Most code mentioning 'Bundle' lies in `fwRuntime`. I realized a lot of code present in that library was never used outside. It would have been useless to double every method and class for non-public code.

I've been keeping saying we do not clearly separate public and private API in our libraries. I decided so to try to achieve that in fwRuntime. After doing this, the only deprecation of the public API will be faster.

To separate the private and the public API in _Sight_ **libraries**, I propose to add `detail` folders both in `include` and `src` folders. Symbols in `detail` folders will be hidden/not exported, thus reducing the size of the shared library and speeding up debugging (when doing this at a large scale of course). This has many advantages in terms of readability of code, maintenance (like this deprecation), etc...

However, one drawback is that since symbols are not exported, it was no longer possible to unit-test private classes and methods. To overcome this, I propose to compile libraries in two steps. One `OBJECT_LIBRARY`, then the usual `SHARED_LIBRARY`, that uses the previous as input of course. This way, the unit-test can build directly with the `OBJECT_LIBRARY` (so the precompiled .obj) directly, removing the need of export, and allowing the test of private classes and methods.

**But**, this was not that simple. In our tests that are often more functional tests than unit-tests, we face two issues:
- we have circular dependencies between libraries, so the test may try to link against both the OBJECT_LIBRARY and the SHARED_LIBRARY, causing awful runtime errors,
- we use lots of static singletons to register types, factories, etc... The same singleton may be instanced both in the OBJECT_LIBRARY and the SHARED_LIBRARY, ending up with a doublon and not a singleton.

The first issue can be tackled, I tried at some point, but it was quite hard, especially as soon as we load modules. The second is even harder sometimes. At the end I chose an alternative. I propose to split tests in two. One for the public API (ex: `fwRuntimeTest`) and one for the implementation (ex: `fwRuntimeImplTest`). The implementation test should be much more minimal and should not require many dependencies, thus reducing the possibility of these two problems to occur. At least that's my hope. :candle:

### fwServices

*Remove the old API from fwServices.*

Remove all the deprecated methods from IService and its associated helpers and registries.
Remove IService default object.

**Note**: the default auto-connection on the first service's object to the 'modified' signal is removed. Check your log to know if your auto-connections are connect.

### guiQt

*Improve SParameters.*

* Fix a bad call to `m_illum`.
* Allow to avoid the automatic update of `SParameters`.
* Add option in `SParameters` to hide the reset button.

### Image

*Improve the API of ::fwData::Image.*

Improve the API of `::fwData::Image`:

* Add the pixel format information: GRAY_SCALE, RGB, RGBA, BGR, BGRA
  * Add an iterator for the different formats
  * the format involve the number of components
* Prevent to use more than 3 dimensions: the size, spacing and origin are defined in a `std::array`
  * 4D images are no longer allowed
* The ``::fwData::Array``is no longer accessible from the image
* Update Dispatcher to use ``::fwTools::Type``instead of ``::fwTools::DynamicType``and deprecate DynamicType.
* **Most** of the old API is still supported with deprecated warning:
  * the new getter/setter for size, origin and spacing are temporary post-fixed by `2` (ex: getSize2())
* Increase the version of Medical Data to serialize the image format (V12, V14AR, V16RD)
* Update `WARNINGS_AS_ERRORS` macro to allow deprecated warnings.

Example:

```cpp
`::fwData::Image::sptr`img = ::fwData::Image::New();

img->resize({1920, 1080}, ::fwTools::Type::s_UINT8, ::fwData::Image::PixelFormat::RGBA);

typedef `::fwData::Image::RGBAIteration`RGBAIteration;

auto lock    = img->lock(); // to prevent buffer dumping
auto iter    = img->begin<RGBAIteration>();
const auto iterEnd = img->end<RGBAIteration>();

for (; iter != iterEnd; ++iter)
{
    iter->r = static_cast<std::uint8_t>(rand()%256);
    iter->g = static_cast<std::uint8_t>(rand()%256);
    iter->b = static_cast<std::uint8_t>(rand()%256);
    iter->a = static_cast<std::uint8_t>(rand()%256);
}
```

### IO

*Use the new Image, Mesh and Array API.*

- Use the new Image, Array and Mesh API in readers and writers.
- Fix Mesh iterators to properly access the values, even if the variable is const.
- Update Image to add the missing API for lazy readers : it requires access to the buffer object to read streams.

*Use the new Image, Mesh and Array API.*

- Refactor fwVtkIO, fwItkIO, igtlProtocol and associated bundles to use the new Image, Mesh and Array API.
- Fix Image iterator: the end of the const iterator was not correct.
- Improve mesh iterator to allow point and cell colors with 3 components (RGB).

### iterator

*Improve iterator to access values.*

- Update Image and Array iterators to allow to access values on const iterators.
- Refactor Image iterator to remove complicated format and only use simple struct.
  - removed warnings when getting an iterator on a type different from the array type. It allows to iterate through a multiple value structure at the same time.

Now, to iterate through an Array or an Image, you can use a struct like:

```cpp

struct RGBA{
    std::uint8t r;
    std::uint8t g;
    std::uint8t b;
    std::uint8t a;
}
```

Then:

```cpp

`::fwData::Image::sptr`image = ::fwData::Image::New();
image->resize(125, 125, 12, ::fwTools::Type::s_UINT8, ::fwData::Image::RGBA);

auto itr = image->begin< RGBA >();
const auto itrEnd = image->end< RGBA >();

for (; itr != itrEnd ; ++itr)
{
    itr->r = 12.0;
    itr->g = 12.0;
    itr->b = 12.0;
    itr->a = 12.0;
}
```

### media

*Fuse all media bundles.*

* Deprecate `arMedia` bundle.
* Copy all `arMedia` content into `media`.
* Fixes xml files that used these bundles.

### Mesh

*Improve the API of ::fwData::Mesh.*

Refactor the mesh API:
- deprecate the access to the points, cells and other arrays
- rename allocate method to reserve
   - allow to allocate the color, normal and texture arrays in the same reserve method
- add resize method to allocate the memory and set the number of points and cells
- add iterator to iterate through the points and cells

**Allocation**:
The two methods `reserve()` and `resize()` allow to allocate the mesh arrays. The difference between the two methods is
that resize modify the number of points and cells.

- `pushPoint()` and `pushCell()` methods allow to add new points or cells, it increments the number of points and allocate the memory if needed.
- `setPoint()` and `setCell()` methods allow to change the value in a given index.

Example with `resize()`, `setPoint()` and `setCell()`:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();

    mesh->resize(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setPoint(i, static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(i, color);
        mesh->setPointNormal(i, normal);
        mesh->setPointTexCoord(i, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        mesh->setCell(i, i, i+1, i+2);

        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setCellColor(i, color);
        mesh->setCellNormal(i, normal);
        mesh->setCellTexCoord(i, texCoords);
    }
```

Example with `reseve()`, `pushPoint()` and `pushCell()`

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();

    mesh->reserve(NB_POINTS, NB_CELLS, CELL_TYPE, EXTRA_ARRAY);
    const auto lock = mesh->lock();

    for (size_t i = 0; i < NB_POINTS; ++i)
    {
        const std::uint8_t val                                            = static_cast<uint8_t>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        const size_t value                                   = 3*i;
        const auto id =
            mesh->pushPoint(static_cast<float>(value), static_cast<float>(value+1), static_cast<float>(value+2));
        mesh->setPointColor(id, color);
        mesh->setPointNormal(id, normal);
        mesh->setPointTexCoord(id, texCoords);
    }

    for (size_t i = 0; i < NB_CELLS; ++i)
    {
        const auto id = mesh->pushCell(i, i+1, i+2);

        const `::fwData::Mesh::ColorValueType`val                          = static_cast< `::fwData::Mesh::ColorValueType`>(i);
        const std::array< ::fwData::Mesh::ColorValueType, 4> color        = {val, val, val, val};
        const float floatVal                                              = static_cast<float>(i);
        const std::array< ::fwData::Mesh::NormalValueType, 3> normal      = {floatVal, floatVal, floatVal};
        const std::array< ::fwData::Mesh::TexCoordValueType, 2> texCoords = {floatVal, floatVal};
        const size_t value                                                = 3*i;
        mesh->setCellColor(id, color);
        mesh->setCellNormal(id, normal);
        mesh->setCellTexCoord(id, texCoords);
    }
```

**Iterators**

To access the mesh points and cells, you should uses the following iterators:
- `::fwData::iterator::PointIterator:`to iterate through mesh points
- `::fwData::iterator::ConstPointIterator:`to iterate through mesh points read-only
- `::fwData::iterator::CellIterator:`to iterate through mesh cells
- `::fwData::iterator::ConstCellIterator:`to iterate through mesh cells read-only

Example to iterate through points:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter    = mesh->begin< `::fwData::iterator::PointIterator`>();
    const auto iterEnd = mesh->end< `::fwData::iterator::PointIterator`>();
    float p[3] = {12.f, 16.f, 18.f};

   for (; iter != iterEnd; ++iter)
   {
       iter->point->x = p[0];
       iter->point->y = p[1];
       iter->point->z = p[2];
   }
```

Example to iterate through cells:

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::TRIANGLE);
    auto iter         = mesh->begin< `::fwData::iterator::ConstCellIterator`>();
    const auto endItr = mesh->end< `::fwData::iterator::ConstCellIterator`>();

    auto itrPt = mesh->begin< `::fwData::iterator::ConstPointIterator`>();
    float p[3];

    for(; iter != endItr; ++iter)
    {
        const auto nbPoints = iter->nbPoints;

        for(size_t i = 0 ; i < nbPoints ; ++i)
        {
            auto pIdx = static_cast< `::fwData::iterator::ConstCellIterator::difference_type`>(iter->pointIdx[i]);

            `::fwData::iterator::ConstPointIterator`pointItr(itrPt + pIdx);
            p[0] = pointItr->point->x;
            p[1] = pointItr->point->y;
            p[2] = pointItr->point->z;
        }
    }
```

`pushCell()` and `setCell()` may not be very efficient, you can use `CellIterator` to define the cell. But take care to
properly define all the cell attribute.

Example of defining cells usign iterators

```cpp
    `::fwData::Mesh::sptr`mesh = ::fwData::Mesh::New();
    mesh->resize(25, 33, ::fwData::Mesh::QUAD);
    auto it          = mesh->begin< `::fwData::iterator::CellIterator`>();
    const auto itEnd = mesh->end< `::fwData::iterator::CellIterator`>();

    const auto cellType = ::fwData::Mesh::QUAD;
    const size_t nbPointPerCell = 4;

    size_t count = 0;
    for (; it != itEnd; ++it)
    {
        // define the cell type and cell offset
        (*it->type)   = cellType;
        (*it->offset) = nbPointPerCell*count;

        // /!\ define the next offset to be able to iterate through point indices
        if (it != itEnd-1)
        {
            (*(it+1)->offset) = nbPointPerCell*(count+1);
        }

        // define the point indices
        for (size_t i = 0; i < 4; ++i)
        {
            `::fwData::Mesh::CellValueType`ptIdx = val;
            it->pointIdx[i] = ptIdx;
        }
    }
```

### MeshIterator

*Cast cell type into CellType enum instead of using std::uint8_t.*

When iterating through mesh cells, the iterator return the type as a CellType enum instead of a std::uint8_t. It allow to avoid a static_cast to compare the type to the enum.

### OgreViewer

*Recreate the whole application.*

* Cleans `visuOgreAdaptor`.
  * Cleans the documentations of all adaptors and fix it for some of them.
  * Draws border of negatos and allow to enable/disable them from the xml.
* Creates a new `OgreViewer`.
* Adds a missing documentation in `LineLayoutManagerBase`.
* Creates a xml color parser.
* Fixes the fragment info adaptor by listening the viewport instead of the layer.
* Improves the slide view builder

### RayTracingVolumeRenderer

*Separate ray marching, sampling, compositing, and lighting code.*

* Simplifies the volume ray tracing shader to make it more understandable.
* Reduces macro definition combinations.

### resource

*Fuse all media bundles.*

* Adds `arMedia` and `media` into a folder `resource`.
* Adds a new bundle `flatIcon` with flat theme icons.

### SNegato2DCamera

*Improve 2D negato interactions.*

* Add a new adaptor to replace the `Negato2D` interactor style.
* Precisely zoom on a voxel using the mouse cursor.
* Deprecate `::visuOgreAdaptor::SInteractorStyle`.
* Update OgreViewer to use the new adaptor.
* Decouple camera management from the negato adaptor.
* Fix interactors to work within a viewport smaller than the whole render window.
* Fix the camera's aspect ratio when the viewport width and height ratios are not the same.

### STrackballCamera

*Move the trackball interaction to a new adaptor.*

* Decouples the trackball interactor from the layer.
* Moves trackball interactions to a new service.
* Makes 'fixed' interactions the default.
* Deprecate parts of the `SInteractorStyle`.

### Tuto

*Use new API of Image, Mesh and Array.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* Refactor Tuto14 and Tuto16 specific algorithms to use the new API
* Refactor `scene` to use new API and remove some warnings
* Refactor `visuVTKAdaptor` to use new API
* Refactor `itkRegistrationOp` library to use new API

### video

*Video use new data api.*

Refactor some services and libraries to use new Image, Mesh and Array API:

* Refactor `videoCalibration`
* Refactor trackers, frame grabbers
* Refactor `cvIO`: conversion between openCV and sight

### visuVTKAdaptor

*Use the new Image, Mesh and Array API.*

* update `fwDataTools` to remove the last uses of the deprecated API except the helpers
* refactor `visuVTKAdaptor` `vtkSimpleNegato`and `vtkSimpleMesh` to remove the deprecated API
* refactor `opImageFilter` and the associated library to remove the deprecated API
* fix `::scene2D`::SComputeHistogram``due to a missing include and remove the deprecated API
* fix ``::fwRenderOgre::Utils``due to a missing include and remove the deprecated API for the image conversion

### VRWidgetsInteractor

*Move the clipping interaction to a separate class.*

* Refactors vr widget interaction. Splits `VRWidgetsInteractor` into `TrackballInteractor` and `ClippingBoxInteractor` and deprecates it.
* Adds right mouse button interaction to the trackball.
* Renames `VRWidget` to `ClippingBox`.

## New features:

### ActivityLauncher

*Update the activityLauncher configuration to change icons paths and readers configuration.*

Allow to customize the activity wizard used by the sequencer. You can use different icons and/or define the readers to use.

* Add a parameter in ActivityLauncher configuration `WIZARD_CONFIG` to define the custom configuration.
* Update ExActivities sample to customize the wizard.

### ARCalibration

*Load calibration input image folders.*

* Add a service to load image folders with calibration inputs.
* Move the detection method to the `calibration3d` library.

*Live reprojection for intrinsic calibration.*

* Compute the reprojection error for each new frame following the calibration.
* Display the reprojected points and the reprojection error.
* Display the detected points.
* Add the ability to undistort the video images once the calibration is computed.
* Modify SSolvePnP to always update the camera parameters.

### build

*Embed PDB file when installing Sight in debug mode on Windows platform.*

Add `install(FILES $<TARGET_PDB_FILE:${FWPROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR}/${FW_INSTALL_PATH_SUFFIX} OPTIONAL)`

### ci

*Build sight-sdk-sample with the SDK.*

* Refactor a bit linux release and debug jobs to build in SDK mode and install/package the SDK. The SDK is an artifact of the job and passed to other dependent jobs
* Add jobs that use the SDK artifact and unpack and use it to build sight-sample-sdk
* The SDK artifact can be used to manually deploy the SDK on artifactory

*Use LFS repo for sight-data.*

This replaces the usage of sight-data with a new repository with LFS support. This speed-ups the download time because git LFS can parallelize batch downloads. This is particularly useful for the CI:
- Previously we used `curl` to retrieve the archive then `tar` to decompress the archive, **the whole process took around 170s**.
- The new `git clone` commands, **this takes only 45s**.

For some ci tasks like deploying issues and merge requests templates, we will use `GIT_LFS_SKIP_SMUDGE=1` which allows to skip completely the download of binaries.

*Remove macos support.*

* remove macos jobs in gitlab-ci script
* add warning message to inform users that macos is no longer supported

### cmake

*Conan update and compilation flag sharing.*

* Update ogre to patched version 1.12.2
  ** Match shader input/ouput attribute names in vertex/geometry/fragment shaders.
  ** Fix varying parsing for the volume proxy bricks shaders.
  ** Replace the deprecated scene node iterator.
  ** Remove the debug plugin configuration on windows.
  ** Explicitly convert quaternions to matrices.
  ** Fix normal rendering.

* Add Visula studio 2019 support with conan package available on artifactory. All conan packages have been updated

* Share "optimized" debug compiler flags across all conan package and Sight. Some previous work on array API (and in many place in our code or our dependencies) showed very bad performance in debug. To mitigate this, we want to use optimized debug build with `-Og -g` (unix) or `/Ox /Oy- /Ob1 /Z7 /MDd` (windows) which will effectively make the speed almost reach release build, while being "debuggable".

  The drawbacks are:
  ** the build will take a bit longer (max 10% longer on gcc/clang, 30% on MSVC)
  ** some lines may be "optimized" out
  ** we loose /RTC1 on windows

  To allow "full" debugging on Sight, we plan to add a special option to use regular flags.

A new option `THOROUGH_DEBUG` has been added to allow full debugging without optimization, if needed. On windows it also add /sdl /RTC1 which performs additional security checks.

Also be sure to have the latest version of conan and that you use the latest `settings.yml` on Windows or macOS (sometimes there is a `settings.yml.new` and you need to rename it `settings.yml`). On Linux, Sight automatically download an updated `settings.yml`, but you can still update conan.

To update conan: `pip3 install --upgrade conan`

*C++17 support.*

This updates our Sight build to support C++17 standard.

Some modifications were also needed from some of our dependencies:
- Camp (patched upstream)
- ITK (updated to 5.0.1)
- Flann (imported patch from upstream)
- Sofa (updated to 19.06.01 and patched locally)

### conan

*Update Qt, PCL, Opencv & Eigen.*

*Update conan package to support CUDA 7.5 arch.*

This is mainly for supporting correctly NVIDIA RTX GPU

### conan-deps

*Support Visual compiler 16 for Visual Studio 2019.*

Now Sight can be compiled with the latest Visual compiler 16 provided
with Visual Studio 2019.

### core

*Use RAII mechanism and weak_prt / shared_ptr pattern to protect fwData against race condition and memory dumping.*

Three class has been added: `weak_ptr`, `locked_ptr`, `shared_ptr`. Each of them will "store" the pointer to the real data object, but in a different manner and with a different way to access it.

You will receive a `weak_ptr` when calling `IServices::getWeak[Input|Inout|Output]()` and a a `locked_ptr` when calling `IServices::getLocked[Input|Inout|Output]()`

weak_ptr will use a hidden `std`::weak_ptr``to hold the real data, but it can only be accessed by "locking" it through a `locked_ptr`. `locked_ptr` will hold a `std::shared_ptr`, but also a mutex lock to guard against concurrent access and a buffer lock to prevent dumping on disk when the data object have an array object. RAII mechanism will ensure that everything is unlocked once the `locked_ptr` is destroyed. The underlying lock mutex will be a **write** mutex locker if the pointer is **NOT const**, a **read** mutex locker if the pointer **is const**. Using an `Input` will anyway force you to have const pointer, forcing you to use a **read** mutex locker. You can simply get the original data object through a std`::shared_ptr`by calling `locked_ptr::getShared()`

*Simplifies macros in fwCore and enables WARNINGS_AS_ERRORS in some core libs.*

* Warnings in Sight core was mainly due to the call of macro with fewer parameters than required.
* We have now a macro `fwCoreClassMacro` with three versions:
  * fwCoreClassMacro(_class);
  * fwCoreClassMacro(_class, _parentClass);
  * fwCoreClassMacro(_class, _parentClass, _factory);
* The macro `fwCoreServiceClassDefinitionsMacro` used in services has been simplified and replaced by `fwCoreServiceMacro(_class, _parentClass);`
* All old deprecated macros have been placed in the `macros-legacy.hpp` file (still included by `macros.hpp` for now - until **Sight 22.0**)
* List of new projects that no longer generate warnings during compilation (new cmake option `WARNINGS_AS_ERRORS` enabled):
  * arData
  * arDataTest
  * fwCom
  * fwComTest
  * fwCore
  * fwCoreTest
  * fwData
  * fwDataTest
  * fwMedData
  * fwMedDataTest
  * fwRuntime
  * fwRuntimeTest
  * fwServices
  * fwServicesTest
  * fwTools
  * fwToolsTest
  * fwRenderOgre
  * fwRenderOgreTest

### ctrlPicking

*Add a new bundle for picking operation services.*

* merge `::uiVisuOgre::SAddPoint`, ``::echoSimulation::STransformPickedPoint``and ``::uiMeasurement::SManageLandmarks``into `::ctrlPicking::SManagePoint`
* Allow to have a maximum number of points in a pointlist via `::opPicking::SAddPoint`.
* Allow to avoid points to be removed in `::ctrlPicking::SManagePoint`.
* Deprecate `::uiVisuOgre::SAddPoint`.
* Deprecate `::uiMeasurement::SManageLandmarks`.

### debian

*Build sight on Debian-med.*

Added several patches to build sight on Debian-med

* fix launcher library path
* fix version getter: in debian workflow, Sight version should be passed from the build parameters and not from the git repository
* do not install conan deps since we use system libraries
* add missing copy constructor for fwData`::ImageIterator`(new warning in gcc9)
* fix support of VTK 7
* remove redundant move in return statement in fwCom \[-Werror=redundant-move\]
* fix build flag for project using system lib
* add a new debian gitlab-ci build job
* fix the source path of dcmtk scp config file

### ExActivitiesQml

*Implement activities for Qml applications.*

Create a Qml sample to launch activities: `ExActivitiesQml`

* Create base class for activity launcher and sequencer service to share the code between qml and qt services.
* Move qml style from `guiQml` to `styleQml` bundles, it is required because some bundles require it and we don't want to start guiQml bundle before them.
* Improve `AppManager` to manage input parameters and to generate unique identifier
* Create an Qml object `ActivityLauncher` to help launching activities in a Qml application

### flatdark

*Update flatdark.qss to add style for persia.*

Modify `flatdark.qss` to add specific style for `Persia` application.

### fwData

*Allow locked_ptr to work with nullptr.*

- If a data is optional, it can be null. But the AppConfig manager try to get a locked_ptr of the data, and the locked_ptr try to lock a nullptr, so an exception is thrown. Now, locked_ptr allows nullptr.

- Fix a crash when clicking on `Reinitialize` or `Delete` in the Volume TF editor.

*Get/set a 4x4 Matrix from TranformationMatrix3D.*

- Add new functions to get/set a TransformationMatrix3D from/to a 4x4 Matrix (array of array). This ensure the row major order and avoid linear to 2d conversions before setting coefficients.

### fwGui

*Manage more gui aspects.*

* Manage label of SCamera.
* Remove a "pading" warning in flatdark.qss.
* Add tooltips on button in the WindowLevel.
* Change the color of disabled QLabel.
* Allow to set the size of each borders in layouts, and manage spacing between widget in the same layout.
* Fix the icons aliasing in the wizard.
* Fix the opacity in `fwGuiQt`

*Manage inverse actions.*

Handle the inverse actions behavior.

When the inverse is set, it means that the behavior of toolbar button (or menu item) is inverted. It is like if it's internal state return false when it is checked and true when unchecked.

*Add parameter to specify the tool button style in ToolBar.*

Enable to display the text beside or under the icons in a toolBar.

### fwGuiQt

*Allow to set a background color on toolbars, menus and layouts.*

### fwPreferences

*Add password management capabilities.*

This MR allow to specify a password, retrieve it from memory (it is stored in a scrambled form) and compare its sha256 hash to the one stored in preferences. This password is then used by SWriter and SReader to write and read encrypted data.

The code rely on fwPreferences helper and should be accessible from anywhere as static variable is used to hold the password in memory. This functions should be thread safe.

### fwRenderOgre

*Simplify fullscreen toggling.*

* Make fullscreen toggling easier and more efficient.
* Add slots to enable/disable fullscreen on the render service.
* Add an action service to select a screen for fullscreen rendering.
* Add a shortcut to return to windowed mode.
* Fix bundle linking in `uiVisuOgre`.

*Configurable viewports.*

* Configure viewports for each layer.
* Refactor the overlay system to enable overlays per viewport (layer).
* Fix adaptors to render text using the new overlay system.

*Allow pickers to pick both side of meshes.*

Take into account the culling mode of entities for the picking.
It allows to know which sides of the triangle to pick.

See Sight/sight#373

### fwRenderOgre/Text

*Crisp font rendering.*

* Improve font rendering by taking into account the dpi and rendering text at the font size.
* Update the dpi on text objects when switching to a screen with a different DPI.
* Handle vertical alignment for texts.
* Update all adaptors to use this new font rendering method.

### fwServices

*Raise an exception when IService::getLockedInput() return a NULL locked_ptr.*

Throw an exception when `IService::getLockedXXX()` retrieve a NULL data object.

### fwZip

*Add "encryption" support when reading and writing jsonz archives.*

- add encryption and update internal minizip to version "1.2".

### gui

*Add notification popups in sight applications.*

* Notification can be displayed at 7 fixed positions (from top-left to bottom right including top & bottom centers).
* 3 Types of notifications: INFO, SUCCESS, FAILURE (background color changes).
* Notifications appears / disappears with a fade-in/out effect on opacity.
* Two ways of displaying notifications:
  * Centralized by the new `SNotifier` service:
    * Using new IService signals `infoNotified`, `successNotified` and `failureNotified` connected to SNotifier slots `popInfo`, `popSuccess`, `popfailure`.
    * `SNotifier` can queue multiple notifications, if queue is full the oldest one is removed.
    * `SNotifier` handles also position of the notification per application/config (ex: always at TOP_RIGHT).
  * Or notification can be displayed by calling directly `::fwGui::NotificationDialog::show()`, loosing the advantages of the centralized system (position, queue, ...).
* Add `ExNotifications` sample & `SDisplayTestNotification` service to test and to show how it can work.

### ImageSeries

*Add more attributes to our medical data.*

* Adds more attributes to our medical data folder.
* Creates patch related to these data.
* Add tests related to these new data versions.

### MeshFunctions

*Add function to convert point coordinates to barycentric one in tetrahedron.*

Add functions and test to convert a points coordinates to barycentric coordinates to its barycentric coordinate inside of a tetrahedron. Add corresponding unit test.

*Add function to convert world coordinates to barycentric.*

* Add functions and test to convert from world coordinates to barycentric coordinates if a point belongs to a triangle.
* Add corresponding unit test.

### MeshPickerInteractor

*New service for mesh picking interactions.*

### OpenVSlam

*Integrate OpenVSLAM in sight.*

* openvslamIO is a library that contains some conversion classes & functions in order to easily convert from/to sight to/from openvslam structures. OpenvslamIO is unit tested.

* openvslamTracker contains SOpenvslam service to interface with openvslam process
  * start/stop/pause/resume tracking
  * Get camera pose for each frames given to the service
  * Get the map as a `::fwData::Mesh`pointcloud
  * Save/Load the map.
  * Save Trajectories as .txt files (matrix or vector/quaternions format).

* ExOpenvslam is an example that show all possibilities that offer the openvlsam integration in sight.

**Note**: this is a preliminary integration so for now openvslam runs only on linux and with monocular perspectives cameras (very classical models).

### SActivitySequencer

*Add new editor to display an activity stepper.*

The list of the activities are displayed in a toolbar, the buttons are enabled only if the activity can be launched (if all its parameters are present). The sequencer uses the data from the previously launched activity to create the current one.

Create a new `ActivityLauncher` configuration to simplify the launch of an activity sequencer.

Improve `ExActivities` sample to use `ActivitySequencer` configuration and add volume rendering activity

### SAxis

*Add a configurable marker.*

Add a configurable marker on `visuOgreAdaptor::SAxis`

### SFrameMatrixSynchronizer

*Add new signals to be consistent with SMatrixTLSynchronizer.*

  -  Add slots to be compatible with the baviour of `SMatrixTLSynchronizer` in order to connect with a `SStatus` service. The `ExStereoARCV` example is updated to show its usage.

  -  The configuration of `SMatrixTLSynchronizer` is updated to take into account the wish to send the status of some matrices in timelines in `SMatrixSynchronizer`.

  -  Now, it's possible to add the tag `sendStatus` in a `<key uid="matrixX" sendStatus="true|false"/>`.

  -  In this way, if you have multiple timelines and multiple matrices, you can choose which one you want to send its status like:

```xml
<inout group="matrices0">
   <key uid="matrix0" sendStatus="true"/>
   <key uid="matrix1" /> <!-- sendStatus is set to false by default -->
   <key uid="matrix2" />
</inout>
<inout group="matrices1">
   <key uid="matrix3" sendStatus="true"/>
   <key uid="matrix4" />
</inout>
```

### SImage

*Add editor to display an image.*

Create an editor ``::guiQt::editor::SImage``to display an image in a view.

### SImageMultiDistance

*Improve interactions and resources management.*

* Fixe `::visuOgreadaptor::SImageMultiDistance`.
* Add auto-snap on distances.
* Update distance on multiple scene.
* Improve resources management.
* Use our new interactor API.

### SLight

*Manage point light.*

* Adds point light in addition of directional light.
* Allows to manage point light position with `SLightEditor`.
* Fixes the specular color computation in `Lighting.glsl`.
* Allows to manage ambient color of materials in a new service `SOrganMaterialEditor`.
* Deprecates the service `OrganMaterialEditor`.

### SMesh

*Add default visibility configuration.*

* Add default visibility state in configuration of a mesh.
* Add a usage example of this feature in `ExSimpleARCVOgre`

### SMultipleTF

*Add new adaptor to manage TF composite.*

Creates a new adaptor to display a composite of TF and interact with them.

The following actions are available:

* Left mouse click: selects a new current TF or move the current clicked TF point.
* Left mouse double click: adds a new TF point to the current TF or open a color dialog to change the current clicked TF point.
* Middle mouse click: adjusts the transfer function window/level by moving the mouse left/right and up/down respectively.
* Right mouse click: remove the current clicked TF point or open a context menu to manage multiple actions which are 'delete', 'add ramp', 'clamp' or 'linear'.

### SNegato2D

*Take the slide position into account.*

* Take into account the slice position to have the right picking information.
* Retrieve the viewport size in the event method for `SNegato2D`.

### SParameters

*Manage dependencies.*

Adds dependencies system on `SParameters`.

### SPointList

*Add slots to update and toggle visibility.*

* Add a `toggleVisibility` slot in `SPointList`
* Implement this feature in `ExOpenvslam`

### SPreferencesConfiguration

*Add the possibility to specify a file in the preferences.*

### SRGBDImageMasking

*New service to perform depth image masking.*

### SSeriesSignal

*Listen the modified signal.*

### SSquare

*Deferred position update and mouse interaction toggling.*

- Implement deferred position update, via the "autoRefresh" xml attribute
- Allow to enable/disable mouse interaction, via the "interaction" xml attribute

### SsquareAdaptor

*Add slots on scene2d SSquare.*

Add a slot to the scene2D Ssquare adaptor which allows the change the configuration parameters of the square. This will allow the movement of the square by changing the values of `X` and `Y` on the widget slider.

### STextStatus

*Add input string.*

Add an input ``::fwData::String``and display it.

### Stransform

*Check if the matrix is null.*

Check if the given matrix of `visuOgreAdaptor`::STransform``is null to avoid a computation error with Ogre's nodes.

### style

*Add QLabel error style.*

Add a style in the qss to make a QLabel red and bold for errors.

*Add service and preference to switch from one theme to another.*

- Selected style is saved in preference file (if it exists), and then reloaded when you re-launch the app.

### SVector

*Add parameter to configure the vector visibility on start.*

Add `visible` parameter in `SVector` configuration to manage the visibility of the vector on start.

### SVideo

*Add update visibility on SVideo Ogre adaptor.*

Add a `updateVisibility` slot in `visuOgreAdaptor::SVideo`

### SVolumeRender

*Buffer the input image in a background thread.*

* Add a new option to load input images into textures in another thread.
* Add a new worker type able to handle graphical resources in parallel.

### Tuto06Filter

*Update the tutorial to follow the API changes (fwData and RAII).*

* Improve Tuto06Filter documentation according to sight-doc.
* Use new service API in SThreshold
* Remove useless dump locks in SThreshold
* Use SSlotCaller action instead of calling the operator service in the menu
* Add some comments

### ui

*Improve gui aesthetic.*

* Improve the background management in 'fwGuiQt'.
* Disable parameters in 'uiMeasurement::SLandmarks' instead of hide them.
* Allow to configure the style of 'SActivityWizard'.
* Poperly use the 'hideActions' mode on toolbars.
* Properly set bounding boxes on 'visuOgreAdaptor::SAxis'.
* Add a flatdark theme.

### uiMeasurementQT

*Allow to remove landmarks with SLandmarks.*

* Allows to add or remove landmarks from picking information with the service 'uiMeasurementQt::editor::SLandmarks'.
* Updates the documentation.

### visuOgreAdaptor

*Improve the negato camera.*

Compute the camera ratio relatively to the viewport size

*Add a punch tool to the volume rendering.*

Adds a punch tool, a new adaptor to creates extruded meshes from a lasso tools, and a service to remove image voxels that are in meshes.

* The service `SShapeExtruder` works as follows:
  * The drawn shape is stored as a points list belonging to a 2D plane in the near of the camera, and a second one is stored in the far of the camera.
  * Once the shape is closed, a triangulation is done on the two points list with a constrained Bowyer-Watson algorithm.
  * Then, for each segment of the shape, two triangles are created between the segment at the near plane and the far plane.

*New adaptor to output renderTarget as an image.*

Add a new adaptor `SFragmentsInfo` that takes informations of the configured layer like, informations can be color, depth or primitive ID.

Some minors updates are also pushed in this MR:

1. SMaterial can now be configured with a `representatioMode` (SURFACE/POINT/EDGE/WIREFRAME).
1. Add new conversion function in `fwRenderOgre`::helper::Camera``to convert from screen space to NDC or viewspace. deprecates the old ones.

*Directional light editing.*

Adds a visual feedback to light adaptors.

*Allow to manage TF windowing with SNegato2DCamera.*

* Add TF windowing management with SNegato2DCamera.
* Fix a configuring error from SPicker.

*Add new adaptors to resize a viewport.*

* Add a new compositor to draw borders in viewports.
* Add an adaptor to resize viewports

*Interact with the 3D negato.*

* Add the same interactions on the negato as in VTK
* Add a priority to the image interactors
* Add the ability to cancel interactions
* Deprecate the old selection interactor API

*Add a new adaptor to display a vector.*

Add `SVector`, a new adaptor that displays a vector.

### visuOgreQt

*Properly compute cameras aspect ratio.*

If an Ogre layer has a ratio different of 1:1, the camera aspect ratio of the layer is badly computed. It's due to the resize event that retrieves the wrong viewport ratio.

*Use qt to create the OpenGL contexts for ogre.*

* Add a new class to create OpenGL contexts for ogre.
* Move the offscreen rendering manager to `visuOgreQt` and instantiate it through a factory.
* Fix material management for the VR widgets.

## Performances:

### videoCalibration

*Speedup chessboard detection.*

* Add a scaling factor to the input to run the detection algorithm on a downscaled image.
* Const-overload `::cvIO::moveToCV`.
* Add a new preference parameter to configure the scaling in ARCalibration.


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



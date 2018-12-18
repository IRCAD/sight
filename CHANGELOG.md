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



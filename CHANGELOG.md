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

### glm

*Add missing GLM_ENABLE_EXPERIMENTAL define.*

Unused glm extensions have been removed


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

## Documentation:

### eigenTools

*Document helper namespace.*

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



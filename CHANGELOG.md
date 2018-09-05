# fw4spl 17.2.0

## Refactor:

### ut

*Replace deprecated methods to register a service.*

Replace the `::OSR::registerService(obj, srv)` by `srv->registerInOut(obj, key)`
in the unit tests.

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

## New features:

### VRRender

*Add activity to upload DICOM series via DicomWeb protocol.*

New activity that anonymizes and uploads DICOM series onto an Orthanc PACS.

### proxyConnection

*Catch exception when the connection failed.*

Catch the exception raised when a connection failed between signals/slots
defined in the configuration.
It displays a error log with the signal/slot information.

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




# fw4spl 17.0.0

## Bug fixes:

### VRRender

*Do not crash when clicking on the distance button in VR.*

The service `::uiMeasurement::editor::Distance`was also cleaned a bit, and the unused configuration option 'placeInscene' was removed.

### docset

*Generation on case sensitive systems.*

### SMesh

*Lock the input mesh properly in slots.*

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



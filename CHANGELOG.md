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



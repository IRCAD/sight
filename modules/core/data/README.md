# sight::module::data

This module contains contains several interfaces for manager, updater and wrapper that work on objects.

## Services

- **MedicalImageSrv**: converts its attached image to a medical image by adding specific fields.
- **SCopy**: copies an object and updates its own objects or the defined target.
- **SExtractDeviceInfo**: extracts camera information in configuration using android device name
- **SExtractMeshByType**: extracts meshes of a specific structure type from data::ModelSeries.
- **SExtractObj**: exposes objects from a source as new objects.
- **SManage**: manages an object (add/swap/remove) into a container object (composite, vector, seriesDB).
- **SMergeTF**: merges all TF from a composite into one TF.
- **SeriesSignals**: emits a signal for each added series (the signal contains the added series)
- **SObjFromSlot**: registers/unregisters the object given by the slots.
- **SImageSignalForwarder**: forwards signals from an image to another.
- **SGetSeries**: extracts series from series database.

## CMake

```cmake

add_dependencies(myTarget 
                ...
                 module_data
)

```
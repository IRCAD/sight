# sight::module::data

This module contains contains several interfaces for data management in XML configurations.

## Services

- **SCopy**: copies an object and updates its own objects or the defined target.
- **SExtractDeviceInfo**: extracts camera information in configuration using android device name
- **SManage**: manages an object (add/swap/remove) into a container object (composite, vector, seriesDB).
- **SMergeTF**: merges all TF from a composite into one TF.
- **SeriesSignals**: emits a signal for each added series (the signal contains the added series)
- **SSelectObject**: registers/unregisters the object given by the slots.
- **SImageSignalForwarder**: forwards signals from an image to another.
- **SGetImage**: extracts an image from a image series.
- **SGetCamera**: extracts camera parameters from a camera series.
- **SGetVector**: extracts objects from a vector.
- **SGetSeries**: extracts series from series database.
- **SGetMesh**: extracts meshes from model series.


## CMake

```cmake

add_dependencies(myTarget
                ...
                 module_data
)

```
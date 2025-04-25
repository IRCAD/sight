# sight::module::data

This module contains contains several interfaces for data management in XML configurations.

## Services

- **copy**: copies an object and updates its own objects or the defined target.
- **manage**: manages an object (add/swap/remove) into a container object (map, vector, series_set).
- **SeriesSignals**: emits a signal for each added series (the signal contains the added series)
- **select_object**: registers/unregisters the object given by the slots.
- **SGetImage**: extracts an image from a image series.
- **get_camera**: extracts camera parameters from a camera series.
- **get_vector**: extracts objects from a vector.
- **get_series**: extracts series from series database.
- **get_mesh**: extracts meshes from model series.


## CMake

```cmake

add_dependencies(my_target
                ...
                 module_data
)

```
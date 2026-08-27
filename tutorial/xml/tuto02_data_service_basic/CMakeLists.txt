sight_add_target(tuto02_data_service_basic TYPE APP)

add_dependencies(
    tuto02_data_service_basic
    data # Allow the use of data objects in the application
    module_app
    module_io_vtk # Provides the reader and writer for VTK files (image and mesh).
    module_service
    module_ui # Provides the user interface framework
    module_ui_qt
    module_viz_sample # Loads basic rendering services for images and meshes.
    sightrun
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto02_data_service_basic)

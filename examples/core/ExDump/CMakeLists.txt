sight_add_target(ExDump TYPE APP)

add_dependencies(
    ExDump
    sightrun
    module_app
    module_ui
    module_ui_qt
    data
    module_service
    module_ui
    module_ui_flaticons
    module_io_itk
    module_io_vtk
    module_io_dicom
    module_io_matrix
    module_io_session
    module_memory
    activity_viz_negato
    module_io_activity
    module_debug
    module_ui_debug
)

# Set the reading mode to "lazy", so the buffers will only be read in module_memory when they are used.
module_param(module_memory PARAM_LIST loading_mode PARAM_VALUES lazy)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExDump_AppCfg)

sight_generate_profile(ExDump)

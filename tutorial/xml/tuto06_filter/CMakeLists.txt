sight_add_target(tuto06_filter TYPE APP)

add_dependencies(
    tuto06_filter
    data
    module_app
    module_filter_image # Provides services to manipulates images, here to perform thresholding
    module_io_vtk
    module_service
    module_ui
    module_ui_qt
    module_viz_sample
    sightrun
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto06_filter)

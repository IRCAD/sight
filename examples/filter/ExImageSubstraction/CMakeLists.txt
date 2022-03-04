sight_add_target(ExImageSubstraction TYPE APP)

add_dependencies(
    ExImageSubstraction
    sightrun
    module_appXml
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3dQt
    data
    module_service
    module_ui_base
    module_ui_flaticons
    module_io_vtk
    module_io_matrix
    module_filter_image
    module_data
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES ExImageSubstraction_AppCfg)

sight_generate_profile(ExImageSubstraction)

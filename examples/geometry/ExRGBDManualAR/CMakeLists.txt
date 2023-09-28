sight_add_target(ExRGBDManualAR TYPE APP)

add_dependencies(
    ExRGBDManualAR
    sightrun
    module_appXml
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3dQt
    data
    module_service
    module_ui
    module_ui_flaticons
    module_io_matrix
    module_io_vtk
    module_io_vision
    module_io_video
    module_sync
    module_data
    module_geometry
    module_filter_vision
    config_ui_datamanager
)

# Main application's configuration to launch
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES ExRGBDManualAR_AppCfg)

sight_generate_profile(ExRGBDManualAR)

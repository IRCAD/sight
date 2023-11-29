sight_add_target(ex_rgbd_manual_ar TYPE APP)

add_dependencies(
    ex_rgbd_manual_ar
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
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
module_param(module_app PARAM_LIST config PARAM_VALUES ExRGBDManualAR_AppCfg)

sight_generate_profile(ex_rgbd_manual_ar)

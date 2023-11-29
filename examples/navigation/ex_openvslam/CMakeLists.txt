sight_add_target(ex_openvslam TYPE APP)

# For now openvslam is only available for unix platforms.
if(UNIX)

endif()

add_dependencies(
    ex_openvslam
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
    module_io_vtk
    module_io_video
    module_sync
    module_data
    module_io_vision
    module_navigation_openvslam
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExOpenvslam_AppCfg)

sight_generate_profile(ex_openvslam)

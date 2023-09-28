sight_add_target(Tuto17SimpleAR TYPE APP)

set(MODULE_PATH "${CMAKE_BINARY_DIR}/${SIGHT_MODULE_RC_PREFIX}/Tuto17SimpleAR")

file(DOWNLOAD https://cloud.ircad.fr/s/sjbojZx983MxiLc/download "${MODULE_PATH}/calibration.xml" SHOW_PROGRESS
     EXPECTED_HASH SHA256=2f983ec650e9df2329354c68d9aff3818741dda921c1857692886cf1bbd947b2
)

file(DOWNLOAD https://cloud.ircad.fr/s/74XGSb523SSa6fD/download "${MODULE_PATH}/aruco_tag.m4v" SHOW_PROGRESS
     EXPECTED_HASH SHA256=92ffe4f0f89380985980a5c1c2fb7536543dee237cddd21922ead2c4e83ecbe2
)

add_dependencies(
    Tuto17SimpleAR
    sightrun
    module_appXml
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3dQt
    data
    module_service
    module_ui
    module_ui_flaticons
    module_io_matrix
    module_io_vtk
    module_io_video
    module_io_vision
    module_data
    module_sync
    module_navigation_optics
    module_geometry_vision
    module_geometry
)

# Main application's configuration to launch
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES Tuto17SimpleAR_AppCfg)

sight_generate_profile(Tuto17SimpleAR)

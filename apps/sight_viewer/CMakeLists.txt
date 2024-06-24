sight_add_target(sight_viewer TYPE APP)

add_dependencies(
    sight_viewer
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_viz_scene3d_qt
    module_service
    data
    viz_scene2d
    module_ui
    module_ui_viz
    module_ui_icons
    module_io_itk
    module_io_vtk
    module_io_dicom
    module_io_session
    config_viz_scene2d
    config_viz_scene3d
    config_io_dimse
)

module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES SightViewer_AppCfg)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif(SIGHT_BUILD_TESTS)

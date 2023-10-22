sight_add_target(SightViewer TYPE APP)

add_dependencies(
    SightViewer
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
    module_ui_flaticons
    module_io_itk
    module_io_vtk
    module_io_dicom
    module_io_session
    config_viz_scene2d
    config_viz_scene3d
    config_io_dimse
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES SightViewer_AppCfg)

sight_generate_profile(SightViewer)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

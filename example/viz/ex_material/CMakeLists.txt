sight_add_target(ex_material TYPE APP)

add_dependencies(
    ex_material
    data
    module_app
    module_io_bitmap
    module_io_vtk
    module_service
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3d_qt
    sightrun
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_material_app_cfg)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()

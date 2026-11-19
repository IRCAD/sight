# The smallest application the GUI test engine needs to test itself: a scene, and the same panel in two views.
sight_add_target(ui_test_app TYPE APP)

add_dependencies(
    ui_test_app
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_service
    module_viz_scene3d
    module_viz_scene3d_qt
)

module_param(module_app PARAM_LIST config PARAM_VALUES ui_test_app_cfg)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()

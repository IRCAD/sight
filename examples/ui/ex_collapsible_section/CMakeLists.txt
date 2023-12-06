sight_add_target(ex_collapsible_section TYPE APP)

add_dependencies(ex_collapsible_section sightrun module_app module_ui module_ui_qt module_service)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(ex_collapsible_section PUBLIC ui Qt5::Core Qt5::Gui Qt5::Widgets)

target_link_libraries(ex_collapsible_section PUBLIC service ui_qt)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_collapsible_section_app_cfg)

sight_generate_profile(ex_collapsible_section)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()

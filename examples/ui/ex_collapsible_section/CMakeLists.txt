sight_add_target(ex_collapsible_section TYPE APP)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PUBLIC service ui_qt)

add_dependencies(${SIGHT_TARGET} sightrun module_app module_ui module_ui_qt module_service)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_collapsible_section_app_cfg)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()

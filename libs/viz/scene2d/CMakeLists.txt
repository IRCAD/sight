sight_add_target(viz_scene2d TYPE LIBRARY)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(viz_scene2d PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(viz_scene2d PROPERTIES AUTOMOC TRUE)

target_link_libraries(viz_scene2d PUBLIC core data ui ui_qt viz service)

if(WIN32)
    target_link_libraries(viz_scene2d PUBLIC glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    find_package(glm QUIET REQUIRED)
    target_include_directories(viz_scene2d SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()

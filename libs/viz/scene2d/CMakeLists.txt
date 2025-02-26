sight_add_target(viz_scene2d TYPE LIBRARY)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(viz_scene2d PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(viz_scene2d PROPERTIES AUTOMOC TRUE)

target_link_libraries(viz_scene2d PUBLIC core data ui ui_qt viz service)

target_link_libraries(viz_scene2d PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif()

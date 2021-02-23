fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)

target_include_directories(
    ${FWPROJECT_NAME} 
    PRIVATE ${CMAKE_BINARY_DIR}/module_ui_viz/include
    )

add_dependencies(${FWPROJECT_NAME} 
                 data
                 geometry_data
                 module_service
                 module_ui_base
                 module_ui_viz
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      viz_scene3d
)

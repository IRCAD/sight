fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core Qml REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt53DCore_INCLUDE_DIRS}
    ${Qt53DRender_INCLUDE_DIRS}
    ${Qt53DExtras_INCLUDE_DIRS}
    ${Qt53DInput_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
    ${Qt53DCore_LIBRARIES}
    ${Qt53DRender_LIBRARIES}
    ${Qt53DExtras_LIBRARIES}
    ${Qt53DInput_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 module_ui_qml
                 data
                 module_io_vtk
                 module_ui_base
                 module_service
                 fwlauncher
                 module_viz_qt3d
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      ui_qml
                      service
                      viz_qt3d
)

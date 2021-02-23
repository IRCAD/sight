sight_target( Tuto05EditorQml TYPE APP START ON )


find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Qml_LIBRARIES}
    ${Qt5Quick_LIBRARIES}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_ui_qml
                 fwlauncher
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      service
                      ui_qml
)

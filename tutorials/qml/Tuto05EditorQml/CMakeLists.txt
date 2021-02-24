sight_add_target( Tuto05EditorQml TYPE APP START ON )


find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Quick Qt5::Qml)
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

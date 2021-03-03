sight_add_target( Tuto07SceneQt3DQml TYPE APP START ON )


find_package(Qt5 QUIET COMPONENTS Core Qml REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Qml)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 module_ui_qml
                 data
                 module_io_vtk
                 module_ui_base
                 module_service
                 sightrun
                 module_viz_qt3d
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      ui_qml
                      service
                      viz_qt3d
)

sight_generate_profile(${FWPROJECT_NAME})

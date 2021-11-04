sight_add_target(Tuto07SceneQt3DQml TYPE APP START ON)

find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
target_link_libraries(Tuto07SceneQt3DQml PUBLIC Qt5::Core Qt5::Quick Qt5::Qml)
set_target_properties(Tuto07SceneQt3DQml PROPERTIES AUTOMOC TRUE)

add_dependencies(
    Tuto07SceneQt3DQml
    module_ui_qml
    data
    module_io_vtk
    module_ui_base
    module_service
    sightrun
    module_viz_qt3d
)

target_link_libraries(Tuto07SceneQt3DQml PUBLIC ui_qml service viz_qt3d)

sight_generate_profile(Tuto07SceneQt3DQml)

sight_add_target(Tuto05EditorQml TYPE APP)

find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
target_link_libraries(Tuto05EditorQml PUBLIC Qt5::Core Qt5::Quick Qt5::Qml)
set_target_properties(Tuto05EditorQml PROPERTIES AUTOMOC TRUE)

add_dependencies(Tuto05EditorQml data module_service module_ui_qml sightrun)

target_link_libraries(Tuto05EditorQml PUBLIC data service ui_qml)

sight_generate_profile(Tuto05EditorQml)

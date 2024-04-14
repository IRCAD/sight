sight_add_target(tuto01_basic_qml TYPE APP)

find_package(Qt5 QUIET COMPONENTS Core Qml Quick REQUIRED)
target_link_libraries(tuto01_basic_qml PUBLIC Qt5::Core Qt5::Quick Qt5::Qml)
set_target_properties(tuto01_basic_qml PROPERTIES AUTOMOC TRUE)

add_dependencies(tuto01_basic_qml data module_service module_ui_qml sightrun)

target_link_libraries(tuto01_basic_qml PUBLIC data service ui_qml)

sight_generate_profile(tuto01_basic_qml)

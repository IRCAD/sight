sight_add_target(module_ui_test TYPE MODULE)

set_target_properties(module_ui_test PROPERTIES AUTOMOC TRUE)

find_package(Qt5 QUIET COMPONENTS Widgets Test REQUIRED)
target_link_libraries(module_ui_test PUBLIC core guiTest Qt5::Widgets Qt5::Test)

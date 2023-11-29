sight_add_target(module_ui_qt_ut TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(module_ui_qt_ut PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(module_ui_qt_ut PROPERTIES AUTOMOC TRUE)

add_dependencies(module_ui_qt_ut module_service module_ui module_ui_qt module_app)

target_link_libraries(module_ui_qt_ut PUBLIC utest_data core data service ui_qt)

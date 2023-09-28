sight_add_target(module_ui_qtTest TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(module_ui_qtTest PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(module_ui_qtTest PROPERTIES AUTOMOC TRUE)

add_dependencies(module_ui_qtTest module_service module_ui module_ui_qt)

target_link_libraries(module_ui_qtTest PUBLIC utestData core data service)

sight_add_target(module_ui_debug TYPE MODULE)

find_package(Qt5 QUIET COMPONENTS Core Concurrent Gui Widgets REQUIRED)
find_package(LibXml2 QUIET REQUIRED)

target_link_libraries(module_ui_debug PRIVATE Qt5::Core Qt5::Concurrent Qt5::Gui Qt5::Widgets)

set_target_properties(module_ui_debug PROPERTIES AUTOMOC TRUE)

target_link_libraries(module_ui_debug PUBLIC core data service ui ui_qt)

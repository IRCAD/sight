sight_add_target(module_ui_debug TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Concurrent Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Widgets Qt6::Concurrent)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PUBLIC core data service ui ui_qt)

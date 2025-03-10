sight_add_target(module_io_document TYPE MODULE)

find_package(Qt6 QUIET COMPONENTS Gui REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC Qt6::Gui)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} module_service)

target_link_libraries(${SIGHT_TARGET} PUBLIC core data ui ui_qt service io)

sight_add_target(module_io_document TYPE MODULE)

find_package(Qt5 QUIET COMPONENTS PrintSupport REQUIRED)
target_link_libraries(module_io_document PUBLIC Qt5::PrintSupport)

add_dependencies(module_io_document module_service)

target_link_libraries(module_io_document PUBLIC core data ui ui_qt service io)

sight_add_target(module_io_dicomweb TYPE MODULE)

find_package(Qt5 QUIET COMPONENTS Core Gui Network Widgets REQUIRED)

target_link_libraries(module_io_dicomweb PUBLIC Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets)

set_target_properties(module_io_dicomweb PROPERTIES AUTOMOC TRUE)

add_dependencies(module_io_dicomweb data module_service)

target_link_libraries(
    module_io_dicomweb
    PUBLIC core
           ui
           ui_qt
           io_http
           data
           service
           io
)

sight_target( module_io_dicomweb TYPE MODULE PRIORITY 1 )


find_package(Qt5 QUIET COMPONENTS Core Gui Network Widgets REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Network Qt5::Widgets)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      ui_base
                      module_ui_qt
                      io_http
                      data
                      service
                      io_base
)

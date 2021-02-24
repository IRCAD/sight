sight_add_target( module_io_dimse TYPE MODULE PRIORITY 1 )

find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      ui_base
                      module_ui_qt
                      io_dimse
                      data
                      service
                      io_base
)

sight_add_target( module_io_document TYPE MODULE )

find_package(Qt5 QUIET COMPONENTS PrintSupport REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::PrintSupport)

add_dependencies(${FWPROJECT_NAME} module_service)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      ui_qt
                      service
                      io_base
)

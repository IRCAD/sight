fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS PrintSupport REQUIRED)

fwForwardInclude(
    ${Qt5PrintSupport_INCLUDE_DIRS}
)

fwLink(
    ${Qt5PrintSupport_LIBRARIES}
)

add_dependencies(${FWPROJECT_NAME} 
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      module_ui_qt
                      service
                      io_base
)

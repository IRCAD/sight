sight_target( module_ui_qtTest TYPE TEST )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets Concurrent REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 module_service
                 module_ui_base
                 module_ui_qt
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      utestData
                      core
                      data
                      service
)

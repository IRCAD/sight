sight_add_target( module_activity TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui)

if(NOT ${ENABLE_QML_APPLICATION})
    find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Widgets)
endif()

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      service
)

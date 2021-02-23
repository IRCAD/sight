sight_target( module_activity TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui REQUIRED)

fwForwardInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)

fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
)

if(NOT ${ENABLE_QML_APPLICATION})
    find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
    fwForwardInclude(
        ${Qt5Widgets_INCLUDE_DIRS}
    )
    fwLink(
        ${Qt5Widgets_LIBRARIES}
    )
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

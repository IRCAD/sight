sight_target( module_ui_dicom TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Qt5Gui_LIBRARIES}
    ${Qt5Widgets_LIBRARIES}
)
fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
)

set_target_properties(${FWPROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      filter_dicom
                      ui_dicom
                      ui_base
                      module_ui_qt
                      service
                      io_base
                      data
)

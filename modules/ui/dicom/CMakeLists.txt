sight_add_target( module_ui_dicom TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
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
                      ui_qt
                      service
                      io_base
                      data
)

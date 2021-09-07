sight_add_target( module_ui_dicom TYPE MODULE )


find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
set_target_properties(module_ui_dicom PROPERTIES AUTOMOC TRUE)

add_dependencies(module_ui_dicom 
                 data
                 module_service
)

target_link_libraries(module_ui_dicom PUBLIC 
                      core
                      filter_dicom
                      ui_dicom
                      ui_base
                      ui_qt
                      service
                      io_base
                      data
)

sight_add_target( module_io_dicom TYPE MODULE)

add_dependencies(module_io_dicom
                 data
                 module_ui_dicom
                 module_service
)

target_link_libraries(module_io_dicom PUBLIC
                      core
                      data
                      io_base
                      io_dicom
                      ui_base
                      service
)

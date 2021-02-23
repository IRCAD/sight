fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_ui_dicom
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_base
                      io_dicom
                      ui_base
                      service
)

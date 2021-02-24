sight_add_target( module_io_itkTest TYPE TEST )



fwInclude(
)
fwLink(
)




add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_ui_base
                 module_io_itk
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      service
                      io_base
)

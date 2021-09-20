sight_add_target( module_io_itkTest TYPE TEST)

add_dependencies(module_io_itkTest
                 data
                 module_service
                 module_ui_base
                 module_io_itk
)

target_link_libraries(module_io_itkTest PUBLIC
                      core
                      utestData
                      data
                      service
                      io_base
)

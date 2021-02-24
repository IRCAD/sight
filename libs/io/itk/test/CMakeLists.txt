sight_add_target( io_itkTest TYPE TEST )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      io_itk
                      service
)

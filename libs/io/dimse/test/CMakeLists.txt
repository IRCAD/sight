sight_add_target( io_dimseTest TYPE TEST )



target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_dicom
                      io_dimse
                      utestData
)

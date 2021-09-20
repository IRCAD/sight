sight_add_target( io_dimseTest TYPE TEST WARNINGS_AS_ERRORS OFF)



target_link_libraries(io_dimseTest PUBLIC 
                      core
                      data
                      io_dicom
                      io_dimse
                      utestData
)

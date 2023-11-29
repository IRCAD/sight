sight_add_target(io_dimse_ut TYPE TEST)

target_link_libraries(io_dimse_ut PUBLIC core data io_dicom io_dimse utest_data)

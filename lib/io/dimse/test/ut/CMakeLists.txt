sight_add_target(io_dimse_ut TYPE DOCTEST)

target_link_libraries(io_dimse_ut PRIVATE core data io_dicom io_dimse utest_data)

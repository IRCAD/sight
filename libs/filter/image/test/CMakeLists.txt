sight_add_target(filter_imageTest TYPE TEST WARNINGS_AS_ERRORS OFF)

target_link_libraries(filter_imageTest PUBLIC utestData data io_vtk filter_image)

sight_add_target(filter_image_ut TYPE TEST)

target_link_libraries(filter_image_ut PUBLIC utest_data data io_vtk filter_image)

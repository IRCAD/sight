sight_add_target(module_io_bitmap_ut TYPE TEST)

add_dependencies(module_io_bitmap_ut module_io_bitmap)

target_link_libraries(module_io_bitmap_ut PUBLIC core utest_data data service ui io_bitmap)

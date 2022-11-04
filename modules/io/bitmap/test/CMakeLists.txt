sight_add_target(module_io_bitmapTest TYPE TEST)

add_dependencies(module_io_bitmapTest module_io_bitmap)

target_link_libraries(module_io_bitmapTest PUBLIC core utestData data service ui_base io_bitmap)

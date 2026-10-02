sight_add_target(module_geometry_ut TYPE TEST)

add_dependencies(module_geometry_ut module_geometry)

target_link_libraries(module_geometry_ut PUBLIC service utest)

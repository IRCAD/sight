sight_add_target(module_geometry TYPE MODULE)
add_dependencies(module_geometry data module_service)

target_link_libraries(module_geometry PUBLIC data core service ui geometry_data)

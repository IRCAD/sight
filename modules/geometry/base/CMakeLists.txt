sight_add_target(module_geometry_base TYPE MODULE)

add_dependencies(module_geometry_base data module_service)

target_link_libraries(module_geometry_base PUBLIC data core service ui_base geometry_data)

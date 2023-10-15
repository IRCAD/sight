sight_add_target(module_app TYPE MODULE)

add_dependencies(module_app module_service)

target_link_libraries(module_app PUBLIC app)

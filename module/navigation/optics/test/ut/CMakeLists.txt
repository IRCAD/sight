sight_add_target(module_navigation_optics_ut TYPE DOCTEST)

add_dependencies(${SIGHT_TARGET} data module_service module_data)

target_link_libraries(${SIGHT_TARGET} PUBLIC service data utest)

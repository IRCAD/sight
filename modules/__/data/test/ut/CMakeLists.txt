sight_add_target(module_data_ut TYPE TEST)

add_dependencies(module_data_ut data module_service module_data)

target_link_libraries(module_data_ut PUBLIC service utest)

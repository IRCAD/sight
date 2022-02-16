sight_add_target(module_dataTest TYPE TEST)

add_dependencies(module_dataTest data module_service module_data)

target_link_libraries(module_dataTest PUBLIC service utest)

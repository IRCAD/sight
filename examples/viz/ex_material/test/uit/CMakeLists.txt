sight_add_target(ex_material_uit TYPE GUI_TEST)

add_dependencies(ex_material_uit ex_material)

target_link_libraries(ex_material_uit ui_test utest_data)

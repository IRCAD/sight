sight_add_target(Tuto06GuiQml TYPE APP START ON)

add_dependencies(Tuto06GuiQml data module_service module_ui_base module_ui_qt)

target_link_libraries(Tuto06GuiQml PUBLIC data service ui_qml)

sight_generate_profile(Tuto06GuiQml)

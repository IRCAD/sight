sight_add_target(tuto06_gui_qml TYPE APP)

add_dependencies(tuto06_gui_qml data module_service module_ui module_ui_qml)

target_link_libraries(tuto06_gui_qml PUBLIC data service ui_qml)

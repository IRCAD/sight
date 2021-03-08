sight_add_target( module_ui_qml TYPE MODULE START ON )


find_package(Qt5 QUIET COMPONENTS Core Gui Quick Qml QuickControls2 REQUIRED)
target_link_libraries(module_ui_qml PUBLIC Qt5::Core Qt5::Gui Qt5::Quick Qt5::Qml Qt5::QuickControls2)

set_target_properties(module_ui_qml PROPERTIES AUTOMOC TRUE)
set(QML_IMPORT_PATH "${QML_IMPORT_PATH};${module_ui_qml_RC_BUILD_DIR}" CACHE STRING "Path of the Qml modules" FORCE)

add_dependencies(module_ui_qml 
                 module_ui_base
)

target_link_libraries(module_ui_qml PUBLIC 
                      core
                      ui_qml
                      ui_qt
                      service
)

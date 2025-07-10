sight_add_target(module_ui_qt_ut TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Widgets SvgWidgets Test REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Widgets Qt6::SvgWidgets Qt6::Test)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

add_dependencies(${SIGHT_TARGET} module_service module_ui module_ui_qt module_app)

target_link_libraries(${SIGHT_TARGET} PUBLIC utest utest_data core data service ui_qt)

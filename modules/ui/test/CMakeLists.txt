sight_add_target(module_ui_test TYPE MODULE)

set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)

find_package(Qt6 QUIET COMPONENTS Widgets Test REQUIRED)
target_link_libraries(${SIGHT_TARGET} PUBLIC core ui_test Qt6::Widgets Qt6::Test)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

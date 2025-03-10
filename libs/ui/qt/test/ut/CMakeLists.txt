sight_add_target(ui_qt_test TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Widgets)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(${SIGHT_TARGET} PUBLIC core service ui_qt utest)

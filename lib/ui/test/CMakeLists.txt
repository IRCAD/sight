sight_add_target(ui_test TYPE LIBRARY FAST_DEBUG ON)

target_link_libraries(ui_test PUBLIC Qt6::Widgets Qt6::Test)
set_target_properties(ui_test PROPERTIES AUTOMOC TRUE)
target_compile_definitions(ui_test PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(ui_test PUBLIC core utest utest_data ui)

# The tests of lib/ui/qt that need the real platform of each system, hence a display.
sight_add_target(ui_qt_native_ut TYPE TEST REQUIRE_X ON)

target_link_libraries(ui_qt_native_ut PUBLIC ui_qt ui_test utest)

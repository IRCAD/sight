# The tests of lib/ui/qt that need the real platform of each system. As GUI tests they have their own display.
sight_add_target(ui_qt_uit TYPE GUI_TEST PROFILE NONE PREFIX "native::")

target_link_libraries(ui_qt_uit PUBLIC ui_qt ui_test utest)

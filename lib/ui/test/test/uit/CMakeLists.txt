# The engine tests on the real platform of each system, where a window has a real stacking, focus and activation, and
# a real OpenGL window. As GUI tests they have their own display, so nothing there is the user's.
sight_add_target(ui_test_uit TYPE GUI_TEST PROFILE NONE PREFIX "native::")

target_link_libraries(ui_test_uit PUBLIC ui_test utest)
target_link_libraries(ui_test_uit PRIVATE Qt6::OpenGLWidgets)

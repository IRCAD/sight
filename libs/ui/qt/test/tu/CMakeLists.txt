sight_add_target(ui_qt_test TYPE TEST WARNINGS_AS_ERRORS OFF)


target_compile_definitions(ui_qt_test PUBLIC "-DQT_NO_KEYWORDS")

find_package(Qt5 QUIET COMPONENTS Widgets REQUIRED)
target_link_libraries(ui_qt_test PUBLIC Qt5::Widgets)
set_target_properties(ui_qt_test PROPERTIES AUTOMOC TRUE)

target_link_libraries(ui_qt_test PUBLIC 
                      core
                      service
                      ui_qt
                      utest
)

sight_add_target(ui_test TYPE LIBRARY FAST_DEBUG ON)

set(CMAKE_AUTOMOC ON)
set_target_properties(ui_test PROPERTIES AUTOMOC TRUE)

target_link_libraries(ui_test PUBLIC Qt5::Widgets Qt5::Test)
target_link_libraries(ui_test PUBLIC core utest utest_data ui)

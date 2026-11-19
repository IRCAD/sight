sight_add_target(ui_test_ut TYPE TEST)

target_link_libraries(ui_test_ut PUBLIC ui_test ui_qt utest)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ui_test_ut PRIVATE nlohmann_json::nlohmann_json)

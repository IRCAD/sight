sight_add_target(ui_test TYPE LIBRARY FAST_DEBUG ON)

find_package(Qt6 QUIET COMPONENTS OpenGLWidgets REQUIRED)
target_link_libraries(ui_test PUBLIC Qt6::Widgets Qt6::Test)
target_link_libraries(ui_test PRIVATE Qt6::OpenGLWidgets ui_qt)

# The MCP server of mcp_server.hpp
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ui_test PRIVATE nlohmann_json::nlohmann_json)
set_target_properties(ui_test PROPERTIES AUTOMOC TRUE)
target_compile_definitions(ui_test PUBLIC "QT_NO_KEYWORDS")

target_link_libraries(ui_test PUBLIC core service utest utest_data ui)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ui_test_app)
    add_subdirectory(test/ut)
    add_subdirectory(test/uit)
endif()

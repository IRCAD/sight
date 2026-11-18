sight_add_target(module_ui_qt_ut TYPE TEST)

find_package(Qt6 QUIET COMPONENTS Multimedia Widgets SvgWidgets Test REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE Qt6::Multimedia Qt6::Widgets Qt6::SvgWidgets Qt6::Test)
set_target_properties(${SIGHT_TARGET} PROPERTIES AUTOMOC TRUE)
target_compile_definitions(${SIGHT_TARGET} PUBLIC "QT_NO_KEYWORDS")

# The sound player is internal to the module: its test builds it.
target_sources(${SIGHT_TARGET} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../sound_player.cpp)
target_include_directories(${SIGHT_TARGET} PRIVATE ${PROJECT_SOURCE_DIR})

add_dependencies(${SIGHT_TARGET} module_service module_ui module_ui_qt module_app)

target_link_libraries(
    ${SIGHT_TARGET}
    PUBLIC utest
           utest_data
           core
           data
           io
           service
           ui_qt
           ui_test
)

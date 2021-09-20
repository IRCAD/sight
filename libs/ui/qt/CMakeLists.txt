sight_add_target( ui_qt TYPE LIBRARY PCH pchService WARNINGS_AS_ERRORS OFF)

target_compile_definitions(ui_qt PUBLIC "-DQT_NO_KEYWORDS")

# find_package(Qt5 ... ) is performed in Dependencies.cmake

target_link_libraries(ui_qt PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Concurrent)
set_target_properties(ui_qt PROPERTIES AUTOMOC TRUE)

target_link_libraries(ui_qt PUBLIC 
                      core
                      data
                      ui_base
                      service
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

sight_add_target(io_http TYPE LIBRARY PCH pchData)

find_package(Qt5 QUIET COMPONENTS Core Gui Network REQUIRED)
target_link_libraries(io_http PUBLIC Qt5::Core Qt5::Gui Qt5::Network)
set_target_properties(io_http PROPERTIES AUTOMOC TRUE)

target_link_libraries(io_http PUBLIC core data ui_base ui_qt)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

sight_add_target(io_http_ut TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Network REQUIRED)
target_link_libraries(io_http_ut PUBLIC Qt5::Core Qt5::Gui Qt5::Network)
set_target_properties(io_http_ut PROPERTIES AUTOMOC TRUE)

target_link_libraries(io_http_ut PUBLIC io_http core utest_data data)

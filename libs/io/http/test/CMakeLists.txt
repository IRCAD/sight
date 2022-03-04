sight_add_target(io_httpTest TYPE TEST)

find_package(Qt5 QUIET COMPONENTS Core Gui Network REQUIRED)
target_link_libraries(io_httpTest PUBLIC Qt5::Core Qt5::Gui Qt5::Network)
set_target_properties(io_httpTest PROPERTIES AUTOMOC TRUE)

target_link_libraries(io_httpTest PUBLIC io_http core utestData data)

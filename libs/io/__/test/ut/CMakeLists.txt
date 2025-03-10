sight_add_target(io_ut TYPE TEST)

find_package(ZLIB QUIET REQUIRED)
target_link_libraries(io_ut PRIVATE ZLIB::ZLIB)

target_link_libraries(io_ut PUBLIC core utest_data data io)

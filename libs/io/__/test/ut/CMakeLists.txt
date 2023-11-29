sight_add_target(io_ut TYPE TEST)

find_package(ZLIB QUIET REQUIRED)
target_include_directories(io_ut SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io_ut PRIVATE ${ZLIB_LIBRARIES})

target_link_libraries(io_ut PUBLIC core utest_data data io ui)

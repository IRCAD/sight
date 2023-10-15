sight_add_target(ioTest TYPE TEST)

find_package(ZLIB QUIET REQUIRED)
target_include_directories(ioTest SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(ioTest PRIVATE ${ZLIB_LIBRARIES})

target_link_libraries(ioTest PUBLIC core utest_data data io ui)

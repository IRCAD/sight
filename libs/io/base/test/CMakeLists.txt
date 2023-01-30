sight_add_target(io_baseTest TYPE TEST)

find_package(ZLIB QUIET REQUIRED)
target_include_directories(io_baseTest SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io_baseTest PRIVATE ${ZLIB_LIBRARIES})

target_link_libraries(io_baseTest PUBLIC core utestData data io_base ui_base)

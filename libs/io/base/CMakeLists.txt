sight_add_target( io_base TYPE LIBRARY PCH pchData )

#racy compatibility
target_compile_definitions(io_base PRIVATE PRJ_NAME="${NAME}")

find_package(ZLIB QUIET REQUIRED )
target_include_directories(io_base SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io_base PRIVATE  ${ZLIB_LIBRARIES})

target_link_libraries(io_base PUBLIC core data service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

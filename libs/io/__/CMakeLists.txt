sight_add_target(io TYPE LIBRARY)

#racy compatibility
target_compile_definitions(io PRIVATE PRJ_NAME="${NAME}")

find_package(Boost QUIET COMPONENTS date_time REQUIRED)

find_package(ZLIB QUIET REQUIRED)
target_include_directories(io SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io PRIVATE ${ZLIB_LIBRARIES})

target_link_libraries(io PUBLIC core data service ui Boost::date_time)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

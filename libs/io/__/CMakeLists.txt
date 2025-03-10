sight_add_target(io TYPE LIBRARY)

find_package(Boost QUIET COMPONENTS date_time REQUIRED)
target_link_libraries(io PRIVATE Boost::date_time)

find_package(ZLIB QUIET REQUIRED)
target_link_libraries(io PRIVATE ZLIB::ZLIB)

target_link_libraries(io PUBLIC core data service ui)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)

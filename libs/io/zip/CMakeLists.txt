sight_add_target(io_zip TYPE LIBRARY)
target_link_libraries(io_zip PUBLIC core)
target_link_libraries(io_zip PRIVATE minizip)
target_include_directories(io_zip PRIVATE ${CMAKE_SOURCE_DIR}/3rd-party)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

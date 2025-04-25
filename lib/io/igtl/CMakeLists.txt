sight_add_target(io_igtl TYPE LIBRARY)

find_package(LibArchive REQUIRED)
target_include_directories(io_igtl PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(io_igtl PRIVATE ${LibArchive_LIBRARIES})

if(VCPKG_TARGET_TRIPLET)
    target_link_libraries(io_igtl PUBLIC OpenIGTLink)
else()
    target_link_libraries(io_igtl PUBLIC ${OpenIGTLink_LIBRARIES})
    target_include_directories(io_igtl SYSTEM PUBLIC ${OpenIGTLink_INCLUDE_DIRS})
endif()

target_link_libraries(io_igtl PUBLIC core data service io_zip)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)

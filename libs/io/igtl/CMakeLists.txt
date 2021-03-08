sight_add_target( io_igtl TYPE LIBRARY )



find_package(LibArchive REQUIRED)
target_include_directories(io_igtl PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(io_igtl PRIVATE ${LibArchive_LIBRARIES})

if(VCPKG_TARGET_TRIPLET)
    find_package(OpenIGTLink CONFIG REQUIRED)
    target_link_libraries(io_igtl PRIVATE OpenIGTLink)
else()
    find_package(OpenIGTLink QUIET REQUIRED)
    target_link_libraries(io_igtl PUBLIC ${OpenIGTLink_LIBRARIES})
    target_include_directories(io_igtl PUBLIC ${OpenIGTLink_INCLUDE_DIRS})
endif()


target_link_libraries(io_igtl PUBLIC 
                      core
                      data
                      service
                      io_zip
                      atoms
                      io_atoms
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

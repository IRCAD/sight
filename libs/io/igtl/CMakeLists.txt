sight_target( io_igtl TYPE LIBRARY )



find_package(LibArchive REQUIRED)
target_include_directories(${FWPROJECT_NAME} PRIVATE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${LibArchive_LIBRARIES})

if(VCPKG_TARGET_TRIPLET)
    find_package(OpenIGTLink CONFIG REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE OpenIGTLink)
else()
    find_package(OpenIGTLink QUIET REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ${OpenIGTLink_LIBRARIES})
    target_include_directories(${FWPROJECT_NAME} PUBLIC ${OpenIGTLink_INCLUDE_DIRS})
endif()


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      io_zip
                      atoms
                      io_atoms
)

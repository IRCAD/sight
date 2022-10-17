sight_add_target(data TYPE LIBRARY)

target_link_libraries(data PUBLIC core)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(data PRIVATE gdcmCommon gdcmMSFF)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

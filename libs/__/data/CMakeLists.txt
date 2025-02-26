sight_add_target(data TYPE LIBRARY)

target_link_libraries(data PUBLIC core)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(data PRIVATE gdcmCommon gdcmMSFF)

target_link_libraries(data PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)

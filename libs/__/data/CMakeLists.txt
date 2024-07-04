sight_add_target(data TYPE LIBRARY)

target_link_libraries(data PUBLIC core)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(data PRIVATE gdcmCommon gdcmMSFF)

if(WIN32)
    target_link_libraries(data PUBLIC glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(data SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)

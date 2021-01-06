
fwLoadProperties()

if(VCPKG_TARGET_TRIPLET)
    find_package(igtl-3.0 CONFIG REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE OpenIGTLink)
else()
    find_package(OpenIGTLink QUIET REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OpenIGTLink_LIBRARIES})
    target_include_directories(${FWPROJECT_NAME} PRIVATE ${OpenIGTLink_INCLUDE_DIRS})
endif()

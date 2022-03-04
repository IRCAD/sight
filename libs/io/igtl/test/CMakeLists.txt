sight_add_target(io_igtlTest TYPE TEST)

if(VCPKG_TARGET_TRIPLET)
    find_package(OpenIGTLink CONFIG REQUIRED)
    target_link_libraries(io_igtlTest PRIVATE OpenIGTLink)
endif()

target_link_libraries(io_igtlTest PUBLIC core data utestData io_igtl)

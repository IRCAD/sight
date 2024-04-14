sight_add_target(io_igtl_ut TYPE TEST)

if(VCPKG_TARGET_TRIPLET)
    find_package(OpenIGTLink CONFIG REQUIRED)
    target_link_libraries(io_igtl_ut PRIVATE OpenIGTLink)
endif()

target_link_libraries(io_igtl_ut PUBLIC core data utest_data io_igtl)

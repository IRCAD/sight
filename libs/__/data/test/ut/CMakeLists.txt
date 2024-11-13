sight_add_target(data_ut TYPE TEST FAST_DEBUG ON)

target_link_libraries(data_ut PUBLIC core utest_data data)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(data_ut PRIVATE gdcmCommon gdcmMSFF)

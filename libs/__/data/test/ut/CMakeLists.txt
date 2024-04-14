sight_add_target(data_ut TYPE TEST FAST_DEBUG ON)

target_link_libraries(data_ut PUBLIC core utest_data data)

sight_add_target(dataTest TYPE TEST FAST_DEBUG ON)

target_link_libraries(dataTest PUBLIC core utest_data data)

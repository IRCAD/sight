sight_add_target(io_session_ut TYPE TEST)

# use obj instead of shared library to be able to test private API
target_link_libraries(io_session_ut PUBLIC io_session_obj utest_data geometry_data)

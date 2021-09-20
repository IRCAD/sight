sight_add_target(io_sessionTest TYPE TEST PCH pchData )

# use obj instead of shared library to be able to test private API
target_link_libraries(io_sessionTest PUBLIC io_session_obj utestData geometry_data)

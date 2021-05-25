sight_add_target(io_session TYPE LIBRARY PCH pchData OBJECT_LIBRARY ON WARNINGS_AS_ERRORS ON)
target_link_libraries(io_session_obj PUBLIC io_base io_zip io_vtk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

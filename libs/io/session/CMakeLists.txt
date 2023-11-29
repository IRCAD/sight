sight_add_target(io_session TYPE LIBRARY OBJECT_LIBRARY ON)

target_link_libraries(io_session_obj PUBLIC io io_zip io_vtk io_dicom)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)

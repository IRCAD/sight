sight_add_target(
    io_session
    TYPE LIBRARY
    PCH pchData
    OBJECT_LIBRARY ON
)

target_link_libraries(io_session_obj PUBLIC io_base io_zip io_vtk io_dicom)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

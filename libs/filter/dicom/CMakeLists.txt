sight_add_target( filter_dicom TYPE LIBRARY PCH pchData )

target_include_directories(filter_dicom SYSTEM PRIVATE ${DCMTK_INCLUDE_DIRS})
target_link_libraries(filter_dicom PRIVATE ${DCMTK_LIBRARIES})

target_link_libraries(filter_dicom PUBLIC core data geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

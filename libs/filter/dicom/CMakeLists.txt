sight_add_target(filter_dicom TYPE LIBRARY)

if(${DCMTK_VERSION} VERSION_GREATER_EQUAL 3.6.7)
    target_link_libraries(filter_dicom PRIVATE DCMTK::dcmdata DCMTK::dcmnet DCMTK::dcmimgle)
else()
    target_link_libraries(filter_dicom PRIVATE dcmdata dcmnet dcmimgle)
endif()

target_link_libraries(filter_dicom PUBLIC core data geometry_data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)

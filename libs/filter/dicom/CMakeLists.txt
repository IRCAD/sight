sight_add_target( filter_dicom TYPE LIBRARY PCH pchData )

target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${DCMTK_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${DCMTK_LIBRARIES})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data geometry_data)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)

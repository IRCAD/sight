sight_add_target(filter_dicom_ut TYPE TEST)

find_package(DCMTK QUIET REQUIRED)

if(${DCMTK_VERSION} VERSION_GREATER_EQUAL 3.6.7)
    target_link_libraries(filter_dicom_ut PRIVATE DCMTK::dcmdata)
else()
    target_link_libraries(filter_dicom_ut PRIVATE dcmdata)
endif()

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(filter_dicom_ut PUBLIC gdcmCommon gdcmMSFF)

target_link_libraries(filter_dicom_ut PUBLIC core data io_dicom filter_dicom geometry_data utest_data)

sight_add_target(io_dicom_ut TYPE TEST)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(io_dicom_ut PUBLIC gdcmCommon gdcmMSFF)

if(WIN32)
    target_link_libraries(io_dicom_ut PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(io_dicom_ut SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()

target_link_libraries(io_dicom_ut PUBLIC io_dicom core data utest_data io_zip io)

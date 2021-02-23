sight_target( filter_dicomTest TYPE TEST )


find_package(DCMTK QUIET REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

find_package(GDCM QUIET REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude( ${GDCM_INCLUDE_DIRS} )
fwLink(
    gdcmCommon
    gdcmMSFF
)


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_dicom
                      filter_dicom
                      geometry_data
                      utestData
)

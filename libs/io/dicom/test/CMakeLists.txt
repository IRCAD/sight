fwLoadProperties()

find_package(GDCM QUIET REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      io_dicom
                      core
                      data
                      utestData
                      io_zip
                      io_base
)

sight_add_target( io_dicomTest TYPE TEST )

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(${FWPROJECT_NAME} PUBLIC
    gdcmCommon
    gdcmMSFF
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      io_dicom
                      core
                      data
                      utestData
                      io_zip
                      io_base
)

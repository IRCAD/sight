fwLoadProperties()

find_package(GDCM QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE gdcmMSFF)

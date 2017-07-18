fwLoadProperties()

find_package(DCMTK REQUIRED)
fwForwardInclude( ${DCMTK_config_INCLUDE_DIRS} )
fwLink( ${DCMTK_LIBRARIES} )

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude( ${GDCM_INCLUDE_DIRS} )
fwLink(
    gdcmCommon
    gdcmMSFF
)


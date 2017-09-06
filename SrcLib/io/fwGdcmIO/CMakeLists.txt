fwLoadProperties()

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

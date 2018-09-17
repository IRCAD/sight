fwLoadProperties()

find_package(GDCM QUIET REQUIRED)
include(${GDCM_USE_FILE})
fwInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmMSFF)


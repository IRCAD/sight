fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataIO
    fwMedData
    fwTools
)

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmMSFF gdcmDSED gdcmCommon)

fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwDataIO
    fwMemory
    fwTools
)


find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(
    ${BOOST_INCLUDE_DIRS}
    ${GDCM_INCLUDE_DIRS}
    )
fwLink(
    ${GDCM_LIBRARIES}
    vtkgdcm
    gdcmMSFF
    gdcmDSED
    gdcmCommon
    )


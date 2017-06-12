fwLoadProperties()

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(
    ${GDCM_INCLUDE_DIRS}
)

fwLink(
    ${GDCM_LIBRARIES}
    ${VTK_LIBRARIES}

    vtkgdcm
    gdcmMSFF
    gdcmDSED
    gdcmCommon
)

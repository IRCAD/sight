fwLoadProperties()

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(
    ${BOOST_INCLUDE_DIRS}
    ${GDCM_INCLUDE_DIRS}
    )
fwLink(
    ${GDCM_LIBRARIES}
    gdcmCommon
    gdcmMSFF
    )


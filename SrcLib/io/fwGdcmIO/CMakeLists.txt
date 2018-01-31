fwLoadProperties()

find_package(Boost QUIET COMPONENTS filesystem locale REQUIRED)

find_package(GDCM QUIET REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(${Boost_INCLUDE_DIRS} ${GDCM_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES} ${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

fwLoadProperties()

find_package(Boost COMPONENTS filesystem locale REQUIRED)

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

fwForwardInclude(${Boost_INCLUDE_DIRS} ${GDCM_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES} ${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

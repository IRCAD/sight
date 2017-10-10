fwLoadProperties()

find_package(Boost COMPONENTS filesystem locale REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})
fwForwardInclude(${GDCM_INCLUDE_DIRS})
fwLink(${GDCM_LIBRARIES} gdcmCommon gdcmMSFF)

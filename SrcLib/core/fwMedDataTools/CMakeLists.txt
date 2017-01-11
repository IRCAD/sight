fwLoadProperties()


find_package (Boost COMPONENTS date_time REQUIRED)
fwInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

if(NOT ANDROID)
    find_package(GDCM REQUIRED)
    include(${GDCM_USE_FILE})
    fwInclude(${GDCM_INCLUDE_DIRS})
    fwLink(${GDCM_LIBRARIES} gdcmMSFF)
endif()

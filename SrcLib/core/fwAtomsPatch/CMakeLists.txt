fwLoadProperties()


find_package(CAMP REQUIRED)
if(NOT ANDROID)
    find_package (Boost COMPONENTS log log_setup filesystem REQUIRED)
endif()

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

if(NOT ANDROID)
    fwLink(
        ${CAMP_LIBRARIES}
        ${Boost_LOG_LIBRARY}
        ${Boost_LOG_SETUP_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
    )
else()
    fwLink(
        ${CAMP_LIBRARIES}
        log
    )
endif()


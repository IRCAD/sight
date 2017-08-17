fwLoadProperties()

if(ANDROID)
    find_package (Boost COMPONENTS thread system filesystem date_time chrono REQUIRED)
else()
    find_package (Boost COMPONENTS thread system filesystem date_time log log_setup REQUIRED)
endif()
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwForwardLink(
    ${Boost_CHRONO_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

if(ANDROID)
fwLink(
        log
)

else()
fwLink(
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
)
endif()



fwLoadProperties()

find_package (Boost COMPONENTS thread system filesystem date_time log log_setup REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwForwardLink(
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

fwLink(
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
)



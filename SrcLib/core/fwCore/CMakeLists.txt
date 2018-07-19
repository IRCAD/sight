fwLoadProperties()

find_package(Boost QUIET COMPONENTS thread system filesystem date_time log log_setup REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS})
fwForwardLink(
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
)

fwLink(
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
)


add_definitions(
    -DBOOST_ALL_NO_LIB
    -DBOOST_ALL_DYN_LINK
)
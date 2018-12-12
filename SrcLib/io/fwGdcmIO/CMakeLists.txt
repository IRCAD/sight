fwLoadProperties()

find_package(Boost QUIET COMPONENTS locale REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::locale PUBLIC gdcmMSFF)

fwLoadProperties()
find_package(CppUnit)
find_package(Boost COMPONENTS log system thread filesystem date_time REQUIRED)

fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_LOG_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
)



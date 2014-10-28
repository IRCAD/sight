
fwLoadProperties()

find_package (Boost COMPONENTS thread date_time filesystem REQUIRED)
find_package(CppUnit)
fwInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})




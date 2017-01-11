fwLoadProperties()

find_package (Boost COMPONENTS date_time REQUIRED)
find_package(CppUnit)

fwInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})


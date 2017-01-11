
fwLoadProperties()


find_package (Boost COMPONENTS chrono REQUIRED)
find_package(CppUnit)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})




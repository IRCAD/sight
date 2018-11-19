fwLoadProperties()

find_package(CppUnit QUIET)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

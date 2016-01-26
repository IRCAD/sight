fwLoadProperties()

find_package (Boost COMPONENTS regex REQUIRED)
find_package(CppUnit)
fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
)
fwLink(
    ${Boost_REGEX_LIBRARY}
    ${CPPUNIT_LIBRARY}
)




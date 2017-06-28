fwLoadProperties()
find_package(CppUnit)

fwForwardInclude(
    ${CPPUNIT_INCLUDE_DIR}
)

fwLink(
    ${CPPUNIT_LIBRARY}
)

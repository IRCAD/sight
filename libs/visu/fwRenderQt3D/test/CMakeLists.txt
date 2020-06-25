fwLoadProperties()

find_package(CppUnit QUIET REQUIRED)

fwForwardInclude(
    ${CPPUNIT_INCLUDE_DIR}
)

fwForwardLink(
    ${CPPUNIT_LIBRARY}
)

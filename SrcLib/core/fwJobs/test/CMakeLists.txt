
fwLoadProperties()

find_package(CppUnit REQUIRED)

fwForwardInclude(
    ${CPPUNIT_INCLUDE_DIR}
)

fwForwardLink(
    ${CPPUNIT_LIBRARY}
)

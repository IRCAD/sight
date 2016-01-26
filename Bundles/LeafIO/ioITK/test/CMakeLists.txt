
fwLoadProperties()

find_package(CppUnit)
find_package(Boost COMPONENTS system filesystem REQUIRED)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)
fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)




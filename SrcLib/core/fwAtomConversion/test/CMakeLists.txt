fwLoadProperties()

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")

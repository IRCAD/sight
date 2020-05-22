fwLoadProperties()

# We need this one to test the location
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

find_package(CppUnit QUIET REQUIRED)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})


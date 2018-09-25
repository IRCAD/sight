fwLoadProperties()

# We need this one to test the location
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_RC_PREFIX="${FWBUNDLE_RC_PREFIX}")

find_package(CppUnit QUIET REQUIRED)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

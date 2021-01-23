fwLoadProperties()

# We need this one to test the location
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

find_package(CppUnit QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE CppUnit)

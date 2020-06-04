fwLoadProperties()

# We need this one to test the location
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

find_package(CppUnit QUIET REQUIRED)

fwInclude(${CPPUNIT_INCLUDE_DIR})

target_link_libraries(${FWPROJECT_NAME} PRIVATE ${CPPUNIT_LIBRARY} ${CMAKE_DL_LIBS})

# Link with the library objects instead of
target_link_libraries(${FWPROJECT_NAME} PUBLIC fwRuntime_obj)

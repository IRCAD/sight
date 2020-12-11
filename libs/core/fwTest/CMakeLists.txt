fwLoadProperties()

find_package(CppUnit QUIET CONFIG REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC CppUnit)

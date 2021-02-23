fwLoadProperties()

find_package(CppUnit QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC CppUnit)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
)

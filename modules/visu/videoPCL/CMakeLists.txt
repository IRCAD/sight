fwLoadProperties()

find_package(PCL QUIET REQUIRED COMPONENTS common io)

fwInclude(
    ${PCL_INCLUDE_DIRS}
)

target_link_libraries(${FWPROJECT_NAME} PRIVATE ${PCL_COMMON_LIBRARIES} ${PCL_IO_LIBRARIES})

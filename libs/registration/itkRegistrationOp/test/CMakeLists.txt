fwLoadProperties()

find_package(ITK QUIET COMPONENTS ITKSpatialObjects REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ITKSpatialObjects)


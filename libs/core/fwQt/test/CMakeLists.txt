fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Core REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Qt5::Core)

fwLoadProperties()

find_package(Filesystem REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::date_time Boost::log Boost::log_setup)

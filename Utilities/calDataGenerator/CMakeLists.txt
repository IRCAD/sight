fwLoadProperties()

find_package(Boost QUIET REQUIRED COMPONENTS program_options filesystem)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options Boost::filesystem)

fwLoadProperties()

target_link_libraries(${FWPROJECT_NAME} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::date_time Boost::log Boost::log_setup)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DWIN32_LEAN_AND_MEAN")
endif()

fwLoadProperties()

find_package(Boost QUIET COMPONENTS date_time log log_setup iostreams REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC std::filesystem)
target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::date_time Boost::log Boost::log_setup Boost::iostreams)

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

fwForwardLink(${CAMP_LIBRARIES})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DWIN32_LEAN_AND_MEAN")
    target_link_libraries(${FWPROJECT_NAME} PRIVATE psapi)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${FWPROJECT_NAME} PRIVATE bcrypt)
endif()

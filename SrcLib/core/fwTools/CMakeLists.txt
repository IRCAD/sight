fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::regex)

if(WIN32 AND Boost_VERSION GREATER 106600)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${FWPROJECT_NAME} PRIVATE bcrypt)
endif()

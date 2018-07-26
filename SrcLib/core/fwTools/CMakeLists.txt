fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex signals REQUIRED)
fwLink(${Boost_LIBRARIES})

if(WIN32 AND Boost_VERSION GREATER 106600)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    fwLink(bcrypt)
endif()

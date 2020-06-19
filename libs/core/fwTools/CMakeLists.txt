fwLoadProperties()

if(WIN32)
    # Boost >= 1.67 changes the default random provider on Windows to use BCrypt
    target_link_libraries(${FWPROJECT_NAME} PRIVATE bcrypt)
endif()

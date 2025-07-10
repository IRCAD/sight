sight_add_target(module_io_joystick TYPE MODULE WARNINGS_AS_ERRORS ON)

target_link_libraries(${SIGHT_TARGET} PRIVATE core io_joystick)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()

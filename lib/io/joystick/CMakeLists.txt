sight_add_target(
    io_joystick
    TYPE LIBRARY
    WARNINGS_AS_ERRORS ON
    OBJECT_LIBRARY ON
)

# find_package(SDL2 CONFIG REQUIRED) done in Dependencies.cmake
target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE SDL2::SDL2)

target_link_libraries(${TARGET_OBJECT_LIB} PRIVATE core)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()

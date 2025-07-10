sight_add_target(module_io_joystick_ut TYPE TEST)

add_dependencies(${SIGHT_TARGET} module_io_joystick)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE SDL2::SDL2)

target_link_libraries(${SIGHT_TARGET} PRIVATE CppUnit core service utest io_joystick)

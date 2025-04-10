sight_add_target(io_joystick_ut TYPE TEST)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE SDL2::SDL2)

target_link_libraries(${SIGHT_TARGET} PRIVATE CppUnit core utest io_joystick_obj)

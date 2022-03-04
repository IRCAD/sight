sight_add_target(
    NetworkProxy
    TYPE EXECUTABLE
    CONSOLE ON
    WARNINGS_AS_ERRORS OFF
)

target_link_libraries(NetworkProxy PUBLIC core data io_igtl)

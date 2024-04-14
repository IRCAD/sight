sight_add_target(
    network_proxy
    TYPE EXECUTABLE
    CONSOLE ON
    WARNINGS_AS_ERRORS OFF
)

target_link_libraries(network_proxy PUBLIC core data io_igtl)

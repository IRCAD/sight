sight_add_target(module_io_sessionTest TYPE TEST)


add_dependencies(module_io_sessionTest
                 data
                 module_service
                 module_ui_base
                 module_io_session
)

target_link_libraries(module_io_sessionTest PUBLIC
                      utestData
                      data
                      service
                      ui_base
                      io_session
)

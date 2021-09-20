sight_add_target( module_io_atomsTest TYPE TEST)

add_dependencies(module_io_atomsTest
                 data
                 module_service
                 module_ui_base
                 module_io_atoms
)

target_link_libraries(module_io_atomsTest PUBLIC
                      core
                      utestData
                      data
                      service
                      io_base
                      io_patch
)

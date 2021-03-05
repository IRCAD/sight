sight_add_target( module_io_atoms TYPE MODULE )





add_dependencies(module_io_atoms 
                 module_service
)

target_link_libraries(module_io_atoms PUBLIC 
                      atoms
                      io_atoms
                      io_patch
                      core
                      data
                      ui_base
                      service
                      io_zip
                      io_base
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
